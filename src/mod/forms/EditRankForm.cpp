#include "EditRankForm.h"
#include "../Utils.hpp"
#include "../manager/lang/LanguageManager.h"
#include "../manager/ranks/RanksManager.h"
#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

void EditRankForm::init(Player& player, object::Rank* rank) {
    ll::form::CustomForm form(Utils::strReplace(
        manager::LanguageManager::getTranslate("formEditRankTitle", player.getLocaleCode()),
        "{rankName}",
        rank->getName()
    ));

    form.appendInput(
        "prefix",
        manager::LanguageManager::getTranslate("formEditRankInputPrefix", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputPrefixPlaceholder", player.getLocaleCode()),
        rank->getPrefix()
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getTranslate("formEditRankInputChatFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputChatFormatPlaceholder", player.getLocaleCode()),
        rank->getChatFormat()
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        rank->getScoreTagFormat()
    );

    std::vector<std::string> rankNames = {
        manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode())
    };
    for (const auto& [name, otherRank] : manager::RanksManager::getRanks()) {
        if (rank->getName() == otherRank->getName()) {
            continue;
        }

        rankNames.push_back(name + " - " + otherRank->getPrefix());
    }

    size_t index = 0;
    if (rank->getInheritanceRank().has_value()) {
        std::string inheritanceRankName =
            rank->getInheritanceRank().value()->getName() + " - " + rank->getInheritanceRank().value()->getPrefix();

        auto it = std::find(rankNames.begin(), rankNames.end(), inheritanceRankName);
        if (it != rankNames.end()) {
            index = std::distance(rankNames.begin(), it);
        }
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getTranslate("formEditRankDropdownInheritanceRanks", player.getLocaleCode()),
        rankNames,
        index
    );
    form.appendInput(
        "availableCommands",
        manager::LanguageManager::getTranslate("formEditRankInputAvailableCommands", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputAvailableCommandsPlaceholder", player.getLocaleCode()),
        Utils::separateVector(rank->getAvailableCommands(), ";")
    );

    form.sendTo(
        player,
        [rank](Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) -> void {
            if (reason.has_value()) {
                return;
            }

            std::unordered_map<std::string, object::Rank*> availableRanks = {
                {manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), nullptr}
            };
            for (const auto& [name, otherRank] : manager::RanksManager::getRanks()) {
                if (rank->getName() == otherRank->getName()) {
                    continue;
                }

                availableRanks[(name + " - " + otherRank->getPrefix())] = otherRank;
            }

            std::string   prefix;
            std::string   chatFormat;
            std::string   scoreTagFormat;
            object::Rank* inheritanceRank;
            std::string   availableCommands;

            try {
                prefix          = std::get_if<std::string>(&result->at("prefix"))->data();
                chatFormat      = std::get_if<std::string>(&result->at("chatFormat"))->data();
                scoreTagFormat  = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
                inheritanceRank = availableRanks[std::get_if<std::string>(&result->at("inheritanceRankName"))->data()];
                availableCommands = std::get_if<std::string>(&result->at("availableCommands"))->data();
            } catch (...) {
                player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (prefix.empty() || chatFormat.empty()) {
                player.sendMessage(manager::LanguageManager::getTranslate("formIncorrectData", player.getLocaleCode()));
                return;
            }

            std::vector<std::string> availableCommandsVector = Utils::strSplit(availableCommands, ";");
            if (availableCommands != "null"
                && (availableCommandsVector.empty() || availableCommandsVector.front().empty())) {
                player.sendMessage(manager::LanguageManager::getTranslate(
                    "editRankInvalidFormatAvailableCommands",
                    player.getLocaleCode()
                ));
                return;
            }

            rank->setPrefix(prefix);
            rank->setChatFormat(chatFormat);
            rank->setScoreTagFormat(scoreTagFormat);

            if (availableCommands != "null") {
                rank->setAvailableCommands(availableCommandsVector);
            } else {
                rank->setAvailableCommands({});
            }

            if (inheritanceRank != nullptr) {
                rank->setInheritanceRank(inheritanceRank);
            } else {
                rank->removeInheritanceRank();
            }

            manager::RanksManager::saveChangesRank(*rank);
            player.sendMessage(Utils::strReplace(
                manager::LanguageManager::getTranslate("editRankSuccess", player.getLocaleCode()),
                "{rankName}",
                rank->getName()
            ));
        }
    );
}

} // namespace power_ranks::forms