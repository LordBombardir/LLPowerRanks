#include "SetRankForm.h"
#include "config/ConfigManager.h"
#include "core/MainManager.h"
#include "lang/LanguageManager.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include "utils/Utils.h"

#include <mc/world/level/Level.h>

namespace power_ranks::forms {

void SetRankForm::init(Player& player) {
    ll::form::CustomForm form(LanguageManager::getTranslate("formSetRankTitle", player.getLocaleCode()));

    std::vector<std::string> playerNames = {};
    player.getLevel().forEachPlayer([&playerNames](Player& otherPlayer) -> bool {
        playerNames.push_back(otherPlayer.getRealName());
        return true;
    });

    form.appendDropdown(
        "playerName",
        LanguageManager::getTranslate("formSetRankDropdownPlayers", player.getLocaleCode()),
        playerNames
    );

    nlohmann::ordered_map<std::string, const types::Rank*> ranks = {};
    for (const auto& [name, rank] : RanksManager::getOrderedRanks()) {
        ranks[std::format("{} - {}", name, RankFormatsManager::getPrefixFormat(rank->getName()))] = rank;
    }

    std::vector<std::string> rankNames = {};
    for (const auto& rankName : ranks | std::views::keys) {
        rankNames.push_back(rankName);
    }

    form.appendDropdown(
        "rankName",
        LanguageManager::getTranslate("formSetRankDropdownRanks", player.getLocaleCode()),
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
                playerName = std::get<std::string>(result->at("playerName"));
                rank       = ranks[std::get<std::string>(result->at("rankName"))];
            } catch (...) {
                player.sendMessage(LanguageManager::getTranslate("undefinedError", player.getLocaleCode()));
                return;
            }

            if (ConfigManager::getConfig().superPlayers.contains(playerName)) {
                player.sendMessage(
                    Utils::strReplace(
                        LanguageManager::getTranslate("setRankSuperPlayer", player.getLocaleCode()),
                        "{playerName}",
                        playerName
                    )
                );
                return;
            }

            if (ConfigManager::getConfig().superRanks.contains(rank->getName())) {
                player.sendMessage(LanguageManager::getTranslate("setRankSuperRank", player.getLocaleCode()));
                return;
            }

            if (Player* otherPlayer = player.getLevel().getPlayer(playerName); otherPlayer != nullptr) {
                MainManager::setPlayerRank(*otherPlayer, *rank);
            } else {
                MainManager::setPlayerRankByName(playerName, *rank);
            }

            player.sendMessage(
                Utils::strReplace(
                    LanguageManager::getTranslate("setRankSuccess", player.getLocaleCode()),
                    {"{playerName}", "{rankName}"},
                    {playerName, rank->getName()}
                )
            );
        }
    );
}

} // namespace power_ranks::forms
