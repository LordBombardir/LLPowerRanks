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
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNames.push_back(pair.first + " - " + pair.second->getPrefix());
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

    std::vector<std::string> rankNames = {};
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNames.push_back(pair.first);
    }

    std::string rankName;
    std::string prefix;
    std::string chatFormat;
    std::string scoreTagFormat;
    std::string inheritanceRankName;

    try {
        rankName       = std::get_if<std::string>(&result->at("rankName"))->data();
        prefix         = std::get_if<std::string>(&result->at("prefix"))->data();
        chatFormat     = std::get_if<std::string>(&result->at("chatFormat"))->data();
        scoreTagFormat = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
        inheritanceRankName =
            Utils::strSplit(std::get_if<std::string>(&result->at("inheritanceRankName"))->data(), " - ")[0];
    } catch (...) {
        player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
        return;
    }

    if (manager::RanksManager::getRank(rankName).has_value()) {
        player.sendMessage(manager::LanguageManager::getTranslate("addRankAlreadyExists", player.getLocaleCode()));
        return;
    }

    std::optional<object::Rank*> inheritanceRank = manager::RanksManager::getRank(inheritanceRankName);
    // clang-format off
    // Почему бы не сделать доступ к сырому значению..?
    if (inheritanceRankName != manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()) && !inheritanceRank.has_value()) {
        std::string ranks = "";
        for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = pair.first;
                continue;
            }

            ranks += ", " + pair.first;
        }

        player.sendMessage(Utils::strReplace(manager::LanguageManager::getTranslate(
            "undefinedRank", player.getLocaleCode()),
            {"{rankName}", "{ranks}"},
            {inheritanceRankName, ranks}
        ));
        return;
    }
    // clang-format on

    manager::RanksManager::addRank(rankName, prefix, chatFormat, scoreTagFormat, inheritanceRank);
    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getTranslate("addRankSuccess", player.getLocaleCode()),
        "{rankName}",
        rankName
    ));
}

} // namespace power_ranks::forms