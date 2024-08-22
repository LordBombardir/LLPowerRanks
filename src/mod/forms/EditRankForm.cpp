#include "EditRankForm.h"
#include "../Utils.hpp"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

void EditRankForm::init(Player& player) {
    ll::form::CustomForm form(
        manager::LanguageManager::getInstance()->getTranslate("formEditRankTitle", player.getLocaleName())
    );

    std::vector<std::string> rankNamesFirst = {};
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNamesFirst.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "rankName",
        manager::LanguageManager::getInstance()->getTranslate("formEditRankDropdownRanks", player.getLocaleName()),
        rankNamesFirst
    );
    form.appendInput(
        "prefix",
        manager::LanguageManager::getInstance()->getTranslate("formEditRankInputPrefix", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputPrefixPlaceholder", player.getLocaleName())
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getInstance()->getTranslate("formEditRankInputChatFormat", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputChatFormatPlaceholder", player.getLocaleName()),
        "({prefix}) {playerName}: {message}"
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputScoreTagFormat", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputScoreTagFormatPlaceholder", player.getLocaleName()),
        "{prefix}"
    );

    std::vector<std::string> rankNamesSecond = {
        manager::LanguageManager::getInstance()->getTranslate("dropdownDontPoint", player.getLocaleName())
    };
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNamesSecond.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getInstance()->getTranslate("formEditRankDropdownRanks", player.getLocaleName()),
        rankNamesSecond
    );
    form.appendInput(
        "availableCommands",
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputAvailableCommands", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formEditRankInputAvailableCommandsPlaceholder", player.getLocaleName())
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
        rankName            = Utils::strSplit(*std::get_if<std::string>(&result->at("rankName")), " - ")[0];
        prefix              = *std::get_if<std::string>(&result->at("prefix"));
        chatFormat          = *std::get_if<std::string>(&result->at("chatFormat"));
        scoreTagFormat      = *std::get_if<std::string>(&result->at("scoreTagFormat"));
        inheritanceRankName = Utils::strSplit(*std::get_if<std::string>(&result->at("inheritanceRankName")), " - ")[0];
        availableCommands   = *std::get_if<std::string>(&result->at("availableCommands"));
    } catch (...) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("undefinedError", player.getLocaleName())
        );
        return;
    }

    std::string definedRanks = "";
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
            manager::LanguageManager::getInstance()->getTranslate("undefinedRank", player.getLocaleName()),
            {"{rankName}", "{ranks}"},
            {rankName, definedRanks}
        ));
        return;
    }

    std::optional<object::Rank*> inheritanceRank = manager::RanksManager::getRank(inheritanceRankName);
    // clang-format off
    // Почему бы не сделать доступ к сырому значению..?
    if (inheritanceRankName != manager::LanguageManager::getInstance()->getTranslate("dropdownDontPoint", player.getLocaleName()) && !inheritanceRank.has_value()) {
        player.sendMessage(Utils::strReplace(manager::LanguageManager::getInstance()->getTranslate(
            "undefinedRank", player.getLocaleName()),
            {"{rankName}", "{ranks}"},
            {inheritanceRankName, definedRanks}
        ));
        return;
    }
    // clang-format on

    std::vector<std::string> availableCommandsVector = Utils::strSplit(availableCommands, ";");
    if (availableCommands != "null" && availableCommandsVector.empty()) {
        player.sendMessage(manager::LanguageManager::getInstance()
                               ->getTranslate("editRankInvalidFormatAvailableCommands", player.getName()));
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
        manager::LanguageManager::getInstance()->getTranslate("editRankSuccess", player.getLocaleName()),
        "{rankName}",
        rankName
    ));
}

} // namespace power_ranks::forms