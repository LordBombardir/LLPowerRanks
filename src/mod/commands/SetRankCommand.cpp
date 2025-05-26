#include "SetRankCommand.h"
#include "../Utils.hpp"
#include "../forms/SetRankForm.h"
#include "../manager/MainManager.h"
#include "../manager/ranks/RanksManager.h"
#include <cstddef>
#include <ll/api/memory/Memory.h>
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void SetRankCommand::execute(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const Parameter&                parameter,
    [[maybe_unused]] const Command& _
) {
    // clang-format off
    bool isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    std::string localeCode = isOriginServer ? manager::ConfigManager::getConfig().defaultLocaleCode : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (!isOriginServer && manager::ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        // clang-format on
        output.error(manager::LanguageManager::getTranslate("setRankSuperRank", localeCode));
        return;
    }

    std::optional<object::Rank*> rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks;
        for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = pair.first;
                continue;
            }

            ranks += ", " + pair.first;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getTranslate("undefinedRank", localeCode),
            {"{rankName}", "{ranks}"},
            {parameter.rankName, ranks}
        ));
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
            output.error(manager::LanguageManager::getTranslate("commandSetRankUndefinedPlayer", localeCode));
            return;
        }

        std::string playerName = filter.front().value;
        manager::MainManager::setPlayerRankByName(playerName, *rank.value());

        output.success(Utils::strReplace(
            manager::LanguageManager::getTranslate("setRankSuccess", localeCode),
            {"{playerName}", "{rankName}"},
            {playerName, parameter.rankName}
        ));
        return;
    }

    std::vector<std::string> playerNames;
    for (Player* player : *players.data) {
        // clang-format off
        if (!isOriginServer && manager::ConfigManager::getConfig().superPlayers.contains(player->getRealName())) {
            // clang-format on
            output.error(Utils::strReplace(
                manager::LanguageManager::getTranslate("setRankSuperPlayer", localeCode),
                "{playerName}",
                player->getRealName()
            ));
            continue;
        }

        manager::MainManager::setPlayerRank(*player, *rank.value());
        playerNames.push_back(player->getRealName());
    }

    if (playerNames.empty()) {
        if (!players.empty()) {
            return;
        }

        output.error(manager::LanguageManager::getTranslate("undefinedError", localeCode));
        return;
    }

    if (playerNames.size() == 1) {
        output.success(Utils::strReplace(
            manager::LanguageManager::getTranslate("setRankSuccess", localeCode),
            {"{playerName}", "{rankName}"},
            {playerNames[0], parameter.rankName}
        ));
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

    output.success(Utils::strReplace(
        manager::LanguageManager::getTranslate("commandSetRankSuccessMultiply", localeCode),
        {"{playerNames}", "{rankName}"},
        {playerNamesStr, parameter.rankName}
    ));
}

void SetRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getTranslate("commandSetRankUsing"));
        return;
    }

    forms::SetRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands