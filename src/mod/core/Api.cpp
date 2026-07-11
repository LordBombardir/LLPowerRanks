#include "Api.h"
#include "MainManager.h"
#include "ranks/RanksManager.h"

#include <stdexcept>

namespace power_ranks::api {

std::unordered_map<std::string, const types::Rank*> getRanks() {
    const std::unordered_map<std::string, types::Rank*>& originalMap = RanksManager::getRanks();

    std::unordered_map<std::string, const types::Rank*> newMap;
    newMap.reserve(originalMap.size());

    for (const auto& [key, value] : originalMap) {
        newMap.emplace(key, value);
    }

    return newMap;
}

std::optional<const types::Rank*> getRank(const std::string& name) { return RanksManager::getRank(name); }

const types::Rank& getPlayerRankOrSetDefault(Player& player) { return MainManager::getPlayerRankOrSetDefault(player); }

const types::Rank& getPlayerRankOrSetDefault(const std::string& playerName) {
    return MainManager::getPlayerRankOrSetDefault(playerName);
}

void setPlayerRank(Player& player, const types::Rank& rank) { MainManager::setPlayerRank(player, rank); }

void setPlayerRankByName(const std::string& playerName, const types::Rank& rank) {
    if (playerName.empty()) {
        throw std::invalid_argument("Parameter «playerName» must not be empty!");
    }

    MainManager::setPlayerRankByName(playerName, rank);
}

void setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank) {
    if (xuid.empty()) {
        throw std::invalid_argument("Parameter «xuid» must not be empty!");
    }

    MainManager::setPlayerRankByXuid(xuid, rank);
}

} // namespace power_ranks::api
