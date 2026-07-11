#include "AddRankForm.h"
#include "lang/LanguageManager.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include "utils/Utils.h"

#include <nlohmann/json.hpp>

namespace power_ranks::forms {

void AddRankForm::init(Player& player) {
    ll::form::CustomForm form(LanguageManager::getTranslate("formAddRankTitle", player.getLocaleCode()));

    form.appendInput(
        "rankName",
        LanguageManager::getTranslate("formAddRankInputRankName", player.getLocaleCode()),
        LanguageManager::getTranslate("formAddRankInputRankNamePlaceholder", player.getLocaleCode())
    );
    form.appendInput(
        "prefix",
        LanguageManager::getTranslate("formAddRankInputPrefix", player.getLocaleCode()),
        LanguageManager::getTranslate("formAddRankInputPrefixPlaceholder", player.getLocaleCode())
    );
    form.appendInput(
        "chatFormat",
        LanguageManager::getTranslate("formAddRankInputChatFormat", player.getLocaleCode()),
        LanguageManager::getTranslate("formAddRankInputChatFormatPlaceholder", player.getLocaleCode()),
        "({prefix}) {playerName}: {message}"
    );
    form.appendInput(
        "scoreTagFormat",
        LanguageManager::getTranslate("formAddRankInputScoreTagFormat", player.getLocaleCode()),
        LanguageManager::getTranslate("formAddRankInputScoreTagFormatPlaceholder", player.getLocaleCode()),
        "{prefix}"
    );

    nlohmann::ordered_map<std::string, std::optional<const types::Rank*>> ranks = {
        {LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), std::nullopt}
    };
    for (const auto& [name, rank] : RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, RankFormatsManager::getPrefixFormat(rank->getName()))] = rank;
    }

    std::vector<std::string> rankNames = {};
    for (const auto& rankName : ranks | std::views::keys) {
        rankNames.push_back(rankName);
    }

    form.appendDropdown(
        "inheritanceRankName",
        LanguageManager::getTranslate("formAddRankDropdownRanks", player.getLocaleCode()),
        rankNames
    );

    form.sendTo(
        player,
        [ranks = std::move(
             ranks
         )](Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason) -> void {
            if (reason.has_value()) {
                return;
            }

            std::string                       rankName;
            std::string                       prefix;
            std::string                       chatFormat;
            std::string                       scoreTagFormat;
            std::optional<const types::Rank*> inheritanceRank;

            try {
                rankName        = std::get<std::string>(result->at("rankName"));
                prefix          = std::get<std::string>(result->at("prefix"));
                chatFormat      = std::get<std::string>(result->at("chatFormat"));
                scoreTagFormat  = std::get<std::string>(result->at("scoreTagFormat"));
                inheritanceRank = ranks[std::get<std::string>(result->at("inheritanceRankName"))];
            } catch (...) {
                player.sendMessage(LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (rankName.empty() || prefix.empty() || chatFormat.empty()) {
                player.sendMessage(LanguageManager::getTranslate("formIncorrectData", player.getLocaleCode()));
                return;
            }

            if (RanksManager::getRank(rankName).has_value()) {
                player.sendMessage(LanguageManager::getTranslate("addRankAlreadyExists", player.getLocaleCode()));
                return;
            }

            RanksManager::addRank(rankName, prefix, chatFormat, scoreTagFormat, inheritanceRank);
            player.sendMessage(
                Utils::strReplace(
                    LanguageManager::getTranslate("addRankSuccess", player.getLocaleCode()),
                    "{rankName}",
                    rankName
                )
            );
        }
    );
}

} // namespace power_ranks::forms
