#include "SetRankCommand.h"
#include "../../config/ConfigManager.h"
#include "../../core/MainManager.h"
#include "../../forms/SetRankForm.h"
#include "../../ranks/RanksManager.h"
#include "../../utils/Utils.h"

#include <ll/api/memory/Memory.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/CommandSelectorBase.h>

namespace power_ranks::commands {

void SetRankCommand::execute(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const Parameter&                parameter,
    [[maybe_unused]] const Command& _
) {
    bool        isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    const auto& localeCode     = isOriginServer ? ConfigManager::getConfig().defaultLocaleCode
                                                : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    const auto& rank = RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks;
        for (const auto& [name, otherRank] : RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = name;
                continue;
            }

            ranks += ", " + name;
        }

        output.error(
            Utils::strReplace(
                LanguageManager::getTranslate("undefinedRank", localeCode),
                {"{rankName}", "{ranks}"},
                {parameter.rankName, std::move(ranks)}
            )
        );
        return;
    }

    CommandSelectorResults<Player> players = parameter.player.results(origin);
    if (players.empty()) {
        constexpr ptrdiff_t offset_mNameFilters = offsetof(CommandSelectorBase, mNameFilters);
        const std::vector<InvertableFilter<std::string>>& filter =
            ll::memory::dAccess<std::vector<InvertableFilter<std::string>>>(
                reinterpret_cast<const void*>(&parameter.player),
                offset_mNameFilters
            );

        if (filter.empty() || filter.front().value == "") {
            output.error(LanguageManager::getTranslate("commandSetRankUndefinedPlayer", localeCode));
            return;
        }

        const auto& playerName = filter.front().value;
        MainManager::setPlayerRankByName(playerName, *rank.value());

        output.success(
            Utils::strReplace(
                LanguageManager::getTranslate("setRankSuccess", localeCode),
                {"{playerName}", "{rankName}"},
                {playerName, parameter.rankName}
            )
        );
        return;
    }

    std::vector<std::string> playerNames;
    for (Player* player : *players.data) {
        // clang-format off
        if (!isOriginServer && ConfigManager::getConfig().superPlayers.contains(player->getRealName())) {
            // clang-format on
            output.error(
                Utils::strReplace(
                    LanguageManager::getTranslate("setRankSuperPlayer", localeCode),
                    "{playerName}",
                    player->getRealName()
                )
            );
            continue;
        }

        MainManager::setPlayerRank(*player, *rank.value());
        playerNames.push_back(player->getRealName());
    }

    if (playerNames.empty()) {
        if (!players.empty()) {
            return;
        }

        output.error(LanguageManager::getTranslate("undefinedError", localeCode));
        return;
    }

    if (playerNames.size() == 1) {
        output.success(
            Utils::strReplace(
                LanguageManager::getTranslate("setRankSuccess", localeCode),
                {"{playerName}", "{rankName}"},
                {playerNames[0], parameter.rankName}
            )
        );
        return;
    }

    std::string playerNamesStr;
    for (std::string& playerName : playerNames) {
        if (playerNamesStr.empty()) {
            playerNamesStr = playerName;
            continue;
        }

        playerNamesStr += ", " + playerName;
    }

    output.success(
        Utils::strReplace(
            LanguageManager::getTranslate("commandSetRankSuccessMultiply", localeCode),
            {"{playerNames}", "{rankName}"},
            {playerNamesStr, parameter.rankName}
        )
    );
}

void SetRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(LanguageManager::getTranslate("commandSetRankUsing"));
        return;
    }

    forms::SetRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands
