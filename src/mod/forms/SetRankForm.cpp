#include "SetRankForm.h"
#include "../Utils.hpp"
#include "../manager/MainManager.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>
#include <mc/network/packet/PlayerListEntry.h>
#include <mc/world/level/Level.h>
#include <variant>

namespace power_ranks::forms {

void SetRankForm::init(Player& player) {
    ll::form::CustomForm form(
        manager::LanguageManager::getInstance()->getTranslate("formSetRankTitle", player.getLocaleName())
    );

    std::vector<std::string> playerNames = {};
    for (const std::pair<mce::UUID, PlayerListEntry> pair : player.getLevel().getPlayerList()) {
        playerNames.push_back(pair.second.mName);
    }

    form.appendDropdown(
        "playerName",
        manager::LanguageManager::getInstance()->getTranslate("formSetRankDropdownPlayers", player.getLocaleName()),
        playerNames
    );

    std::vector<std::string> rankNames = {};
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNames.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "rankName",
        manager::LanguageManager::getInstance()->getTranslate("formSetRankDropdownRanks", player.getLocaleName()),
        rankNames
    );

    form.sendTo(player, &handle);
}

void SetRankForm::handle(Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) {
    if (reason.has_value()) {
        return;
    }

    std::string playerName;
    std::string rankName;

    try {
        playerName = *std::get_if<std::string>(&result->at("playerName"));
        rankName   = Utils::strSplit(*std::get_if<std::string>(&result->at("rankName")), " - ")[0];
    } catch (...) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("undefinedError", player.getLocaleName())
        );
        return;
    }

    if (Utils::isValueInVector(manager::ConfigManager::getConfig().superPlayers, playerName)) {
        player.sendMessage(Utils::strReplace(
            manager::LanguageManager::getInstance()->getTranslate("setRankSuperPlayer", player.getLocaleName()),
            "{playerName}",
            playerName
        ));
        return;
    }

    if (Utils::isValueInVector(manager::ConfigManager::getConfig().superRanks, rankName)) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("setRankSuperRank", player.getLocaleName())
        );
        return;
    }

    std::optional<object::Rank*> rank = manager::RanksManager::getRank(rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("undefinedError", player.getLocaleName())
        );
        return;
    }

    if (Player* otherPlayer = player.getLevel().getPlayer(playerName); otherPlayer != nullptr) {
        manager::MainManager::setPlayerRank(*otherPlayer, *rank.value());
    } else {
        manager::MainManager::setPlayerRankByName(playerName, *rank.value());
    }

    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getInstance()->getTranslate("setRankSuccess", player.getLocaleName()),
        {"{playerName}", "{rankName}"},
        {playerName, rankName}
    ));
}

} // namespace power_ranks::forms