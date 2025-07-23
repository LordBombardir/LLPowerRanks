#include "EditRankForm.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/rankFormats/RankFormatsManager.h"
#include "../manager/ranks/RanksManager.h"
#include "../utils/Utils.h"
#include <nlohmann/json.hpp>

namespace power_ranks::forms {

void EditRankForm::init(Player& player, types::Rank* rank, const std::string& localeCode) {
    ll::form::CustomForm form(
        Utils::strReplace(
            manager::LanguageManager::getTranslate("formEditRankTitle", player.getLocaleCode()),
            "{rankName}",
            rank->getName()
        )
    );

    form.appendLabel(
        Utils::strReplace(
            manager::LanguageManager::getTranslate("formEditRankLabel", player.getLocaleCode()),
            "{localeCode}",
            localeCode
        )
    );

    form.appendInput(
        "prefix",
        manager::LanguageManager::getTranslate("formEditRankInputPrefix", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputPrefixPlaceholder", player.getLocaleCode()),
        manager::RankFormatsManager::getRawPrefixFormat(rank->getName(), localeCode)
    );
    form.appendInput(
        "chatFormat",
        manager::LanguageManager::getTranslate("formEditRankInputChatFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputChatFormatPlaceholder", player.getLocaleCode()),
        manager::RankFormatsManager::getRawChatFormat(rank->getName(), localeCode)
    );
    form.appendInput(
        "scoreTagFormat",
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormat", player.getLocaleCode()),
        manager::LanguageManager::getTranslate("formEditRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        manager::RankFormatsManager::getRawScoreTagFormat(rank->getName(), localeCode)
    );

    nlohmann::ordered_map<std::string, std::optional<const types::Rank*>> ranks = {
        {manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), std::nullopt}
    };
    for (const auto& [name, rank] : manager::RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, manager::RankFormatsManager::getPrefixFormat(rank->getName()))] = rank;
    }

    const auto& inheritanceRank = rank->getInheritanceRank();

    size_t                   index     = 0;
    std::vector<std::string> rankNames = {};

    size_t currentIndex = 0;
    for (const auto& [rankName, rank] : ranks) {
        if (inheritanceRank.has_value() && rank.has_value() && *inheritanceRank == *rank) {
            index = currentIndex;
        }

        rankNames.push_back(rankName);
        currentIndex++;
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
        Utils::separateContainer(rank->getAvailableCommands(), ";")
    );
    form.appendInput(
        "hiddenCommandOverloads",
        manager::LanguageManager::getTranslate("formEditRankInputHiddenCommandOverloads", player.getLocaleCode()),
        manager::LanguageManager::getTranslate(
            "formEditRankInputHiddenCommandOverloadsPlaceholder",
            player.getLocaleCode()
        ),
        rank->getHiddenCommandOverloads().toString()
    );
    form.appendInput(
        "additionalInformation",
        manager::LanguageManager::getTranslate("formEditRankInputAdditionalInformation", player.getLocaleCode()),
        manager::LanguageManager::getTranslate(
            "formEditRankInputAdditionalInformationPlaceholder",
            player.getLocaleCode()
        ),
        Utils::separateContainer(rank->getAdditionalInformation(), ";")
    );

    form.sendTo(
        player,
        [rank, localeCode = localeCode, ranks = std::move(ranks)](
            Player&                           player,
            const ll::form::CustomFormResult& result,
            ll::form::FormCancelReason        reason
        ) -> void {
            if (reason.has_value()) {
                return;
            }

            std::string                       prefix;
            std::string                       chatFormat;
            std::string                       scoreTagFormat;
            std::optional<const types::Rank*> inheritanceRank;
            std::string                       availableCommands;
            std::string                       hiddenCommandOverloads;
            std::string                       additionalInformation;

            try {
                prefix                 = std::get_if<std::string>(&result->at("prefix"))->data();
                chatFormat             = std::get_if<std::string>(&result->at("chatFormat"))->data();
                scoreTagFormat         = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
                inheritanceRank        = ranks[std::get_if<std::string>(&result->at("inheritanceRankName"))->data()];
                availableCommands      = std::get_if<std::string>(&result->at("availableCommands"))->data();
                hiddenCommandOverloads = std::get_if<std::string>(&result->at("hiddenCommandOverloads"))->data();
                additionalInformation  = std::get_if<std::string>(&result->at("additionalInformation"))->data();
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
                player.sendMessage(
                    manager::LanguageManager::getTranslate(
                        "editRankInvalidFormatAvailableCommands",
                        player.getLocaleCode()
                    )
                );
                return;
            }

            std::vector<std::string> additionalInformationVector = Utils::strSplit(additionalInformation, ";");
            if (additionalInformation != "null"
                && (additionalInformationVector.empty() || additionalInformationVector.front().empty())) {
                player.sendMessage(
                    manager::LanguageManager::getTranslate(
                        "editRankInvalidFormatAdditionalInformation",
                        player.getLocaleCode()
                    )
                );
                return;
            }

            if (inheritanceRank.has_value()) {
                rank->setInheritanceRank(*inheritanceRank);
            } else {
                rank->removeInheritanceRank();
            }

            if (availableCommands != "null") {
                rank->setAvailableCommands({availableCommandsVector.begin(), availableCommandsVector.end()});
            } else {
                rank->setAvailableCommands({});
            }

            rank->getHiddenCommandOverloads().updateFromString(hiddenCommandOverloads);

            if (additionalInformation != "null") {
                rank->setAdditionalInformation(additionalInformationVector);
            } else {
                rank->setAvailableCommands({});
            }

            manager::RankFormatsManager::setRankFormat(rank->getName(), prefix, chatFormat, scoreTagFormat, localeCode);
            manager::RanksManager::saveChangesRank(*rank);

            player.sendMessage(
                Utils::strReplace(
                    manager::LanguageManager::getTranslate("editRankSuccess", player.getLocaleCode()),
                    "{rankName}",
                    rank->getName()
                )
            );
        }
    );
}

} // namespace power_ranks::forms