#include "AddRankForm.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/rankFormats/RankFormatsManager.h"
#include "../manager/ranks/RanksManager.h"
#include "../utils/Utils.h"
#include <nlohmann/json.hpp>

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

    nlohmann::ordered_map<std::string, std::optional<const types::Rank*>> ranks = {
        {manager::LanguageManager::getTranslate("dropdownDontPoint", player.getLocaleCode()), std::nullopt}
    };
    for (const auto& [name, rank] : manager::RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, manager::RankFormatsManager::getPrefixFormat(rank->getName()))] = rank;
    }

    std::vector<std::string> rankNames = {};
    for (const auto& rankName : ranks | std::views::keys) {
        rankNames.push_back(rankName);
    }

    form.appendDropdown(
        "inheritanceRankName",
        manager::LanguageManager::getTranslate("formAddRankDropdownRanks", player.getLocaleCode()),
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
                rankName        = std::get_if<std::string>(&result->at("rankName"))->data();
                prefix          = std::get_if<std::string>(&result->at("prefix"))->data();
                chatFormat      = std::get_if<std::string>(&result->at("chatFormat"))->data();
                scoreTagFormat  = std::get_if<std::string>(&result->at("scoreTagFormat"))->data();
                inheritanceRank = ranks[std::get_if<std::string>(&result->at("inheritanceRankName"))->data()];
            } catch (...) {
                player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (rankName.empty() || prefix.empty() || chatFormat.empty()) {
                player.sendMessage(manager::LanguageManager::getTranslate("formIncorrectData", player.getLocaleCode()));
                return;
            }

            if (manager::RanksManager::getRank(rankName).has_value()) {
                player.sendMessage(
                    manager::LanguageManager::getTranslate("addRankAlreadyExists", player.getLocaleCode())
                );
                return;
            }

            manager::RanksManager::addRank(rankName, prefix, chatFormat, scoreTagFormat, inheritanceRank);
            player.sendMessage(
                Utils::strReplace(
                    manager::LanguageManager::getTranslate("addRankSuccess", player.getLocaleCode()),
                    "{rankName}",
                    rankName
                )
            );
        }
    );
}

} // namespace power_ranks::forms