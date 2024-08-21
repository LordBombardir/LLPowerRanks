#include "SetRankCommand.h"
#include "../Utils.hpp"
#include "../forms/SetRankForm.h"
#include "../manager/MainManager.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void SetRankCommand::execute(
    const CommandOrigin& origin,
    CommandOutput&       output,
    const Parameter&     parameter,
    const Command&       _
) {
    // clang-format off
    bool isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    std::string localeName = isOriginServer ? manager::ConfigManager::getConfig().defaultLocaleName : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleName();

    CommandSelectorResults<Player> players = parameter.player.results(origin);
    if (players.empty()) {
        output.error(manager::LanguageManager::getInstance()->getTranslate("commandSetRankUndefinedPlayer", localeName));
        return;
    }

    if (!isOriginServer && Utils::isValueInVector(manager::ConfigManager::getConfig().superRanks, parameter.rankName)) {
        // clang-format on
        output.error(manager::LanguageManager::getInstance()->getTranslate("commandSetRankSuperRank", localeName));
        return;
    }

    std::optional<object::Rank*> rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks = "";
        for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = pair.first;
                continue;
            }

            ranks += ", " + pair.first;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getInstance()->getTranslate("commandSetRankUndefinedRank", localeName),
            {"{rankName}", "{ranks}"},
            {parameter.rankName, ranks}
        ));
        return;
    }

    std::vector<std::string> playerNames;
    for (Player* player : *players.data) {
        // clang-format off
        if (!isOriginServer && Utils::isValueInVector(manager::ConfigManager::getConfig().superPlayers, player->getRealName())) {
            // clang-format on
            output.error(Utils::strReplace(
                manager::LanguageManager::getInstance()->getTranslate("commandSetRankSuperPlayer", localeName),
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

        output.error(manager::LanguageManager::getInstance()->getTranslate("commandSetRankUndefinedError", localeName));
        return;
    }

    if (playerNames.size() == 1) {
        output.success(Utils::strReplace(
            manager::LanguageManager::getInstance()->getTranslate("commandSetRankSuccess", localeName),
            {"{playerName}", "{rankName}"},
            {playerNames[0], parameter.rankName}
        ));
        return;
    }

    std::string playerNamesStr = "";
    for (std::string& playerName : playerNames) {
        if (playerNamesStr.empty()) {
            playerNamesStr = playerName;
            continue;
        }

        playerNamesStr += ", " + playerName;
    }

    output.success(Utils::strReplace(
        manager::LanguageManager::getInstance()->getTranslate("commandSetRankSuccessMultiply", localeName),
        {"{playerNames}", "{rankName}"},
        {playerNamesStr, parameter.rankName}
    ));
}

void SetRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getInstance()->getTranslate("commandSetRankUsing"));
        return;
    }

    forms::SetRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands