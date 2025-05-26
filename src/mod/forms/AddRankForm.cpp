#include "AddRankForm.h"
#include "../Utils.hpp"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

void AddRankForm::init(Player& player) {
    ll::form::CustomForm form(manager::LanguageManager::getTranslate("formAddRankTitle", player.getLocaleCode()));

    form.appendInput(
        "rankName",
        manager::LanguageManager::getTranslate("formAddRankInputRankName", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formAddRankInputRankNamePlaceholder", player.getLocaleCode())
    );
    form.appendInput(
        "prefix",
        manager::LanguageManager::getTranslate("formAddRankInputPrefix", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formAddRankInputPrefixPlaceholder", player.getLocaleCode())
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getTranslate("formAddRankInputChatFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formAddRankInputChatFormatPlaceholder", player.getLocaleCode()),
        "({prefix}) {playerName}: {message}"
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getTranslate("formAddRankInputScoreTagFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formAddRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        "{prefix}"
    );

    std::vector<std::string> rankNames = {
        manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode())
    };
    for (const auto& [name, rank] : manager::RanksManager::getRanks()) {
        rankNames.push_back(name + " - " + rank->getPrefix());
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getTranslate("formAddRankDropdownRanks", player.getLocaleCode()),
        rankNames
    );

    form.sendTo(player, &handle);
}

void AddRankForm::handle(Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) {
    if (reason.has_value()) {
        return;
    }

    std::unordered_map<std::string, std::optional<object::Rank*>> availableRanks = {
        {manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), std::nullopt}
    };
    for (const auto& [name, otherRank] : manager::RanksManager::getRanks()) {
        availableRanks[(name + " - " + otherRank->getPrefix())] = otherRank;
    }

    std::string                  rankName;
    std::string                  prefix;
    std::string                  chatFormat;
    std::string                  scoreTagFormat;
    std::optional<object::Rank*> inheritanceRank;

    try {
        rankName        = std::get_if<std::string>(&result->at("rankName"))->data();
        prefix          = std::get_if<std::string>(&result->at("prefix"))->data();
        chatFormat      = std::get_if<std::string>(&result->at("chatFormat"))->data();
        scoreTagFormat  = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
        inheritanceRank = availableRanks[std::get_if<std::string>(&result->at("inheritanceRankName"))->data()];
    } catch (...) {
        player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
        return;
    }

    if (manager::RanksManager::getRank(rankName).has_value()) {
        player.sendMessage(manager::LanguageManager::getTranslate("addRankAlreadyExists", player.getLocaleCode()));
        return;
    }

    manager::RanksManager::addRank(rankName, prefix, chatFormat, scoreTagFormat, inheritanceRank);
    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getTranslate("addRankSuccess", player.getLocaleCode()),
        "{rankName}",
        rankName
    ));
}

} // namespace power_ranks::forms