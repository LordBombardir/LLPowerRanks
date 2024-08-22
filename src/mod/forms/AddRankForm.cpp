#include "AddRankForm.h"
#include "../Utils.hpp"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

void AddRankForm::init(Player& player) {
    ll::form::CustomForm form(
        manager::LanguageManager::getInstance()->getTranslate("formAddRankTitle", player.getLocaleName())
    );

    form.appendInput(
        "rankName",
        manager::LanguageManager::getInstance()->getTranslate("formAddRankInputRankName", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formAddRankInputRankNamePlaceholder", player.getLocaleName())
    );
    form.appendInput(
        "prefix",
        manager::LanguageManager::getInstance()->getTranslate("formAddRankInputPrefix", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formAddRankInputPrefixPlaceholder", player.getLocaleName())
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getInstance()->getTranslate("formAddRankInputChatFormat", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formAddRankInputChatFormatPlaceholder", player.getLocaleName()),
        "({prefix}) {playerName}: {message}"
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getInstance()->getTranslate("formAddRankInputScoreTagFormat", player.getLocaleName()),
        manager::LanguageManager::getInstance()
            ->getTranslate("formAddRankInputScoreTagFormatPlaceholder", player.getLocaleName()),
        "{prefix}"
    );

    std::vector<std::string> rankNames = {manager::LanguageManager::getInstance()->getTranslate(
        "dropdownDontPoint",
        player.getLocaleName()
    )};
    for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
        rankNames.push_back(pair.first + " - " + pair.second->getPrefix());
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getInstance()->getTranslate("formAddRankDropdownRanks", player.getLocaleName()),
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
        rankName            = *std::get_if<std::string>(&result->at("rankName"));
        prefix              = *std::get_if<std::string>(&result->at("prefix"));
        chatFormat          = *std::get_if<std::string>(&result->at("chatFormat"));
        scoreTagFormat      = *std::get_if<std::string>(&result->at("scoreTagFormat"));
        inheritanceRankName = Utils::strSplit(*std::get_if<std::string>(&result->at("inheritanceRankName")), " - ")[0];
    } catch (...) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("undefinedError", player.getLocaleName())
        );
        return;
    }

    if (manager::RanksManager::getRank(rankName).has_value()) {
        player.sendMessage(
            manager::LanguageManager::getInstance()->getTranslate("addRankAlreadyExists", player.getLocaleName())
        );
        return;
    }

    std::optional<object::Rank*> inheritanceRank = manager::RanksManager::getRank(inheritanceRankName);
    // clang-format off
    // Почему бы не сделать доступ к сырому значению..?
    if (inheritanceRankName != manager::LanguageManager::getInstance()->getTranslate("dropdownDontPoint", player.getLocaleName()) && !inheritanceRank.has_value()) {
        std::string ranks = "";
        for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = pair.first;
                continue;
            }

            ranks += ", " + pair.first;
        }

        player.sendMessage(Utils::strReplace(manager::LanguageManager::getInstance()->getTranslate(
            "undefinedRank", player.getLocaleName()),
            {"{rankName}", "{ranks}"},
            {inheritanceRankName, ranks}
        ));
        return;
    }
    // clang-format on

    manager::RanksManager::addRank(rankName, prefix, chatFormat, scoreTagFormat, inheritanceRank);
    player.sendMessage(Utils::strReplace(
        manager::LanguageManager::getInstance()->getTranslate("addRankSuccess", player.getLocaleName()),
        "{rankName}",
        rankName
    ));
}

} // namespace power_ranks::forms