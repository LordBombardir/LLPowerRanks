#include "EditRankForm.h"
#include "lang/LanguageManager.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include "utils/Utils.h"

#include <nlohmann/json.hpp>

namespace power_ranks::forms {

void EditRankForm::init(Player& player, types::Rank* rank, const std::string& localeCode) {
    ll::form::CustomForm form(
        Utils::strReplace(
            LanguageManager::getTranslate("formEditRankTitle", player.getLocaleCode()),
            "{rankName}",
            rank->getName()
        )
    );

    form.appendLabel(
        Utils::strReplace(
            LanguageManager::getTranslate("formEditRankLabel", player.getLocaleCode()),
            "{localeCode}",
            localeCode
        )
    );

    form.appendInput(
        "prefix",
        LanguageManager::getTranslate("formEditRankInputPrefix", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputPrefixPlaceholder", player.getLocaleCode()),
        RankFormatsManager::getRawPrefixFormat(rank->getName(), localeCode)
    );
    form.appendInput(
        "chatFormat",
        LanguageManager::getTranslate("formEditRankInputChatFormat", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputChatFormatPlaceholder", player.getLocaleCode()),
        RankFormatsManager::getRawChatFormat(rank->getName(), localeCode)
    );
    form.appendInput(
        "scoreTagFormat",
        LanguageManager::getTranslate("formEditRankInputScoreTagFormat", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        RankFormatsManager::getRawScoreTagFormat(rank->getName(), localeCode)
    );

    nlohmann::ordered_map<std::string, std::optional<const types::Rank*>> ranks = {
        {LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), std::nullopt}
    };
    for (const auto& [name, r] : RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, RankFormatsManager::getPrefixFormat(r->getName()))] = r;
    }

    const auto& inheritanceRank = rank->getInheritanceRank();

    size_t                   index     = 0;
    std::vector<std::string> rankNames = {};

    size_t currentIndex = 0;
    for (const auto& [rankName, r] : ranks) {
        if (inheritanceRank.has_value() && r.has_value() && *inheritanceRank == *r) {
            index = currentIndex;
        }

        rankNames.push_back(rankName);
        currentIndex++;
    }

    form.appendDropdown(
        "inheritanceRankName",
        LanguageManager::getTranslate("formEditRankDropdownInheritanceRanks", player.getLocaleCode()),
        rankNames,
        index
    );
    form.appendInput(
        "availableCommands",
        LanguageManager::getTranslate("formEditRankInputAvailableCommands", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputAvailableCommandsPlaceholder", player.getLocaleCode()),
        Utils::separateContainer(rank->getAvailableCommands(), ";")
    );
    form.appendInput(
        "hiddenCommandOverloads",
        LanguageManager::getTranslate("formEditRankInputHiddenCommandOverloads", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputHiddenCommandOverloadsPlaceholder", player.getLocaleCode()),
        rank->getHiddenCommandOverloads().toString()
    );
    form.appendInput(
        "additionalInformation",
        LanguageManager::getTranslate("formEditRankInputAdditionalInformation", player.getLocaleCode()),
        LanguageManager::getTranslate("formEditRankInputAdditionalInformationPlaceholder", player.getLocaleCode()),
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
                prefix                 = std::get<std::string>(result->at("prefix"));
                chatFormat             = std::get<std::string>(result->at("chatFormat"));
                scoreTagFormat         = std::get<std::string>(result->at("scoreTagFormat"));
                inheritanceRank        = ranks[std::get<std::string>(result->at("inheritanceRankName"))];
                availableCommands      = std::get<std::string>(result->at("availableCommands"));
                hiddenCommandOverloads = std::get<std::string>(result->at("hiddenCommandOverloads"));
                additionalInformation  = std::get<std::string>(result->at("additionalInformation"));
            } catch (...) {
                player.sendMessage(LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (prefix.empty() || chatFormat.empty()) {
                player.sendMessage(LanguageManager::getTranslate("formIncorrectData", player.getLocaleCode()));
                return;
            }

            std::vector<std::string> availableCommandsVector = Utils::strSplit(availableCommands, ";");
            if (availableCommands != "null"
                && (availableCommandsVector.empty() || availableCommandsVector.front().empty())) {
                player.sendMessage(
                    LanguageManager::getTranslate("editRankInvalidFormatAvailableCommands", player.getLocaleCode())
                );
                return;
            }

            std::vector<std::string> additionalInformationVector = Utils::strSplit(additionalInformation, ";");
            if (additionalInformation != "null"
                && (additionalInformationVector.empty() || additionalInformationVector.front().empty())) {
                player.sendMessage(
                    LanguageManager::getTranslate("editRankInvalidFormatAdditionalInformation", player.getLocaleCode())
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
                rank->setAdditionalInformation({});
            }

            RankFormatsManager::setRankFormat(rank->getName(), prefix, chatFormat, scoreTagFormat, localeCode);
            RanksManager::saveChangesRank(*rank);

            player.sendMessage(
                Utils::strReplace(
                    LanguageManager::getTranslate("editRankSuccess", player.getLocaleCode()),
                    "{rankName}",
                    rank->getName()
                )
            );
        }
    );
}

} // namespace power_ranks::forms
