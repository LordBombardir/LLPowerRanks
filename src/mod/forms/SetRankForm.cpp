#include "SetRankForm.h"
#include "../utils/Utils.h"
#include "../manager/MainManager.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>
#include <mc/world/level/Level.h>
#include <variant>

namespace power_ranks::forms {

void SetRankForm::init(Player& player) {
    ll::form::CustomForm form(manager::LanguageManager::getTranslate("formSetRankTitle", player.getLocaleCode()));

    std::vector<std::string> playerNames = {};
    player.getLevel().forEachPlayer([&playerNames](Player& otherPlayer) -> bool {
        playerNames.push_back(otherPlayer.getRealName());
        return true;
    });

    form.appendDropdown(
        "playerName",
        manager::LanguageManager::getTranslate("formSetRankDropdownPlayers", player.getLocaleCode()),
        playerNames
    );

    std::vector<std::string> rankNames = {};
    for (const auto& [name, rank] : manager::RanksManager::getRanks()) {
        rankNames.push_back(name + " - " + rank->getPrefix());
    }

    form.appendDropdown(
        "rankName",
        manager::LanguageManager::getTranslate("formSetRankDropdownRanks", player.getLocaleCode()),
        rankNames
    );

    form.sendTo(player, &handle);
}

void SetRankForm::handle(Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) {
    if (reason.has_value()) {
        return;
    }

    std::unordered_map<std::string, types::Rank*> availableRanks = {};
    for (const auto& [name, otherRank] : manager::RanksManager::getRanks()) {
        availableRanks[(name + " - " + otherRank->getPrefix())] = otherRank;
    }

    std::string  playerName;
    types::Rank* rank;

    try {
        playerName = std::get_if<std::string>(&result->at("playerName"))->data();
        rank       = availableRanks[std::get_if<std::string>(&result->at("rankName"))->data()];
    } catch (...) {
        player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
        return;
    }

    if (manager::ConfigManager::getConfig().superPlayers.contains(playerName)) {
        player.sendMessage(Utils::strReplace(
            manager::LanguageManager::getTranslate("setRankSuperPlayer", player.getLocaleCode()),
            "{playerName}",
            playerName
        ));
        return;
    }

    if (manager::ConfigManager::getConfig().superRanks.contains(rank->getName())) {
        player.sendMessage(manager::LanguageManager::getTranslate("setRankSuperRank", player.getLocaleCode()));
        return;
    }

    if (Player* otherPlayer = player.getLevel().getPlayer(playerName); otherPlayer != nullptr) {
        manager::MainManager::setPlayerRank(*otherPlayer, *rank);
    } else {
        manager::MainManager::setPlayerRankByName(playerName, *rank);
    }

    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getTranslate("setRankSuccess", player.getLocaleCode()),
        {"{playerName}", "{rankName}"},
        {playerName, rank->getName()}
    ));
}

} // namespace power_ranks::forms