#include "SetRankForm.h"
#include "../manager/MainManager.h"
#include "../manager/lang/LanguageManager.h"
#include "../manager/rankFormats/RankFormatsManager.h"
#include "../manager/ranks/RanksManager.h"
#include "../utils/Utils.h"
#include <mc/world/level/Level.h>
#include <variant>

namespace power_ranks::forms {

void SetRankForm::init(Player& player) {
    ll::form::CustomForm form(manager::LanguageManager::getTranslate("formSetRankTitle", player.getLocaleCode()));

    std::vector<std::string> playerNames = {};
    player.getLevel().forEachPlayer([&playerNames](Player& otherPlayer) -> bool {
        playerNames.push_back(otherPlayer.getRealName());
        return true;
    });

    form.appendDropdown(
        "playerName",
        manager::LanguageManager::getTranslate("formSetRankDropdownPlayers", player.getLocaleCode()),
        playerNames
    );

    nlohmann::ordered_map<std::string, const types::Rank*> ranks = {};
    for (const auto& [name, rank] : manager::RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, manager::RankFormatsManager::getPrefixFormat(rank->getName()))] = rank;
    }

    std::vector<std::string> rankNames = {};
    for (const auto& rankName : ranks | std::views::keys) {
        rankNames.push_back(rankName);
    }

    form.appendDropdown(
        "rankName",
        manager::LanguageManager::getTranslate("formSetRankDropdownRanks", player.getLocaleCode()),
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

            std::string        playerName;
            const types::Rank* rank;

            try {
                playerName = std::get_if<std::string>(&result->at("playerName"))->data();
                rank       = ranks[std::get_if<std::string>(&result->at("rankName"))->data()];
            } catch (...) {
                player.sendMessage(manager::LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (manager::ConfigManager::getConfig().superPlayers.contains(playerName)) {
                player.sendMessage(
                    Utils::strReplace(
                        manager::LanguageManager::getTranslate("setRankSuperPlayer", player.getLocaleCode()),
                        "{playerName}",
                        playerName
                    )
                );
                return;
            }

            if (manager::ConfigManager::getConfig().superRanks.contains(rank->getName())) {
                player.sendMessage(manager::LanguageManager::getTranslate("setRankSuperRank", player.getLocaleCode()));
                return;
            }

            if (Player* otherPlayer = player.getLevel().getPlayer(playerName); otherPlayer != nullptr) {
                manager::MainManager::setPlayerRank(*otherPlayer, *rank);
            } else {
                manager::MainManager::setPlayerRankByName(playerName, *rank);
            }

            player.sendMessage(
                Utils::strReplace(
                    manager::LanguageManager::getTranslate("setRankSuccess", player.getLocaleCode()),
                    {"{playerName}", "{rankName}"},
                    {playerName, rank->getName()}
                )
            );
        }
    );
}

} // namespace power_ranks::forms