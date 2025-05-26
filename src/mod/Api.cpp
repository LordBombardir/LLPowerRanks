#include "LLPowerRanks.h"
#include "manager/MainManager.h"
#include "manager/ranks/RanksManager.h"
#include <stdexcept>

namespace power_ranks::api {

std::unordered_map<std::string, const object::Rank*> getRanks() {
    const std::unordered_map<std::string, object::Rank*>& originalMap = manager::RanksManager::getRanks();

    std::unordered_map<std::string, const object::Rank*> newMap;
    newMap.reserve(originalMap.size());

    for (const auto& [key, value] : originalMap) {
        newMap.emplace(key, value);
    }

    return newMap;
}

std::optional<const object::Rank*> getRank(const std::string& name) { return manager::RanksManager::getRank(name); }

const object::Rank& getPlayerRankOrSetDefault(Player& player) {
    return manager::MainManager::getPlayerRankOrSetDefault(player);
}

const object::Rank& getPlayerRankOrSetDefault(const std::string& playerName) {
    return manager::MainManager::getPlayerRankOrSetDefault(playerName);
}

void setPlayerRank(Player& player, const object::Rank& rank) { manager::MainManager::setPlayerRank(player, rank); }

void setPlayerRankByName(const std::string& playerName, const object::Rank& rank) {
    if (playerName.empty()) {
        throw std::invalid_argument("Parameter «playerName» must not be empty!");
    }

    manager::MainManager::setPlayerRankByName(playerName, rank);
}

void setPlayerRankByXuid(const std::string& xuid, const object::Rank& rank) {
    if (xuid.empty()) {
        throw std::invalid_argument("Parameter «xuid» must not be empty!");
    }

    manager::MainManager::setPlayerRankByXuid(xuid, rank);
}

} // namespace power_ranks::api
