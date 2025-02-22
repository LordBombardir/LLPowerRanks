#include "EditRankForm.h"
#include "../Utils.hpp"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

void EditRankForm::init(Player& player) {
    ll::form::CustomForm form(manager::LanguageManager::getTranslate("formEditRankTitle", player.getLocaleCode()));

    std::vector<std::string> rankNamesFirst = {};
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNamesFirst.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "rankName",
        manager::LanguageManager::getTranslate("formEditRankDropdownRanks", player.getLocaleCode()),
        rankNamesFirst
    );
    form.appendInput(
        "prefix",
        manager::LanguageManager::getTranslate("formEditRankInputPrefix", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputPrefixPlaceholder", player.getLocaleCode())
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getTranslate("formEditRankInputChatFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputChatFormatPlaceholder", player.getLocaleCode()),
        "({prefix}) {playerName}: {message}"
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        "{prefix}"
    );

    std::vector<std::string> rankNamesSecond = {
        manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode())
    };
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNamesSecond.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getTranslate("formEditRankDropdownInheritanceRanks", player.getLocaleCode()),
        rankNamesSecond
    );
    form.appendInput(
        "availableCommands",
        manager::LanguageManager::getTranslate("formEditRankInputAvailableCommands", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputAvailableCommandsPlaceholder", player.getLocaleCode())
    );

    form.sendTo(player, &handle);
}

void EditRankForm::handle(Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) {
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
    std::string availableCommands;

    try {
        rankName       = Utils::strSplit(std::get_if<std::string>(&result->at("rankName"))->data(), " - ")[0];
        prefix         = std::get_if<std::string>(&result->at("prefix"))->data();
        chatFormat     = std::get_if<std::string>(&result->at("chatFormat"))->data();
        scoreTagFormat = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
        // clang-format off
        inheritanceRankName = Utils::strSplit(std::get_if<std::string>(&result->at("inheritanceRankName"))->data(), " - ")[0];
        // clang-format on
        availableCommands = std::get_if<std::string>(&result->at("availableCommands"))->data();
    } catch (...) {
        player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
        return;
    }

    std::string definedRanks;
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        if (definedRanks.empty()) {
            definedRanks = pair.first;
            continue;
        }

        definedRanks += ", " + pair.first;
    }

    std::optional<object::Rank*> rank = manager::RanksManager::getRank(rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        player.sendMessage(Utils::strReplace(
            manager::LanguageManager::getTranslate("undefinedRank", player.getLocaleCode()),
            {"{rankName}", "{ranks}"},
            {rankName, definedRanks}
        ));
        return;
    }

    std::optional<object::Rank*> inheritanceRank = manager::RanksManager::getRank(inheritanceRankName);
    // clang-format off
    // Почему бы не сделать доступ к сырому значению..?
    if (inheritanceRankName != manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()) && !inheritanceRank.has_value()) {
        player.sendMessage(Utils::strReplace(manager::LanguageManager::getTranslate(
            "undefinedRank", player.getLocaleCode()),
            {"{rankName}", "{ranks}"},
            {inheritanceRankName, definedRanks}
        ));
        return;
    }
    // clang-format on

    std::vector<std::string> availableCommandsVector = Utils::strSplit(availableCommands, ";");
    if (availableCommands != "null" && availableCommandsVector.empty()) {
        player.sendMessage(
            manager::LanguageManager::getTranslate("editRankInvalidFormatAvailableCommands", player.getLocaleCode())
        );
        return;
    }

    rank.value()->setPrefix(prefix);
    rank.value()->setChatFormat(chatFormat);
    rank.value()->setScoreTagFormat(scoreTagFormat);
    rank.value()->setAvailableCommands(availableCommandsVector);

    if (inheritanceRank.has_value()) {
        rank.value()->setInheritanceRank(inheritanceRank.value());
    } else {
        rank.value()->removeInheritanceRank();
    }

    manager::RanksManager::saveChangesRank(*rank.value());
    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getTranslate("editRankSuccess", player.getLocaleCode()),
        "{rankName}",
        rankName
    ));
}

} // namespace power_ranks::forms