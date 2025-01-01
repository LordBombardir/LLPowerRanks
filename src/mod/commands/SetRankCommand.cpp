#include "SetRankCommand.h"
#include "../Utils.hpp"
#include "../forms/SetRankForm.h"
#include "../manager/MainManager.h"
#include "../manager/ranks/RanksManager.h"
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

    CommandSelectorResults<Player> players = parameter.player.results(origin);
    if (players.empty()) {
        output.error(manager::LanguageManager::getTranslate("commandSetRankUndefinedPlayer", localeCode));
        return;
    }

    if (!isOriginServer && Utils::isValueInVector(manager::ConfigManager::getConfig().superRanks, std::string{parameter.rankName})) {
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

        output.error(
            Utils::strReplace(
                manager::LanguageManager::getTranslate("undefinedRank", localeCode),
                {"{rankName}", "{ranks}"},
                {parameter.rankName, ranks}
            )
        );
        return;
    }

    std::vector<std::string> playerNames;
    for (Player* player : *players.data) {
        // clang-format off
        if (!isOriginServer && Utils::isValueInVector(manager::ConfigManager::getConfig().superPlayers, player->getRealName())) {
            // clang-format on
            output.error(
                Utils::strReplace(
                    manager::LanguageManager::getTranslate("setRankSuperPlayer", localeCode),
                    "{playerName}",
                    player->getRealName()
                )
            );
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
        output.success(
            Utils::strReplace(
                manager::LanguageManager::getTranslate("setRankSuccess", localeCode),
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
            manager::LanguageManager::getTranslate("commandSetRankSuccessMultiply", localeCode),
            {"{playerNames}", "{rankName}"},
            {playerNamesStr, parameter.rankName}
        )
    );
}

void SetRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getTranslate("commandSetRankUsing"));
        return;
    }

    forms::SetRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands