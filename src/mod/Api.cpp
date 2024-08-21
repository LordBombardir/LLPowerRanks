#include "Api.h"
#include "manager/MainManager.h"
#include "manager/ranks/RanksManager.h"
#include <stdexcept>
#include <utility>


std::unordered_map<std::string, const power_ranks::object::Rank*> getRanks() {
    std::unordered_map<std::string, power_ranks::object::Rank*> originalMap =
        power_ranks::manager::RanksManager::getRanks();
    std::unordered_map<std::string, const power_ranks::object::Rank*> newMap;

    for (const std::pair<std::string, power_ranks::object::Rank*> pair : originalMap) {
        newMap.emplace(pair);
    }

    return newMap;
}
const std::optional<const power_ranks::object::Rank*> getRank(const std::string& name) {
    return power_ranks::manager::RanksManager::getRank(name);
}

const power_ranks::object::Rank& getPlayerRankOrSetDefault(Player& player) {
    return power_ranks::manager::MainManager::getPlayerRankOrSetDefault(player);
}
const power_ranks::object::Rank& getPlayerRankOrSetDefault(const std::string& playerName) {
    return power_ranks::manager::MainManager::getPlayerRankOrSetDefault(playerName);
}

void setPlayerRank(Player& player, const power_ranks::object::Rank& rank) {
    power_ranks::manager::MainManager::setPlayerRank(player, rank);
}
void setPlayerRankByName(const std::string& playerName, const power_ranks::object::Rank& rank) {
    if (playerName.empty()) {
        throw std::invalid_argument("Parameter «playerName» must not be empty!");
    }

    power_ranks::manager::MainManager::setPlayerRankByName(playerName, rank);
}

void setPlayerRankByXuid(const std::string& xuid, const power_ranks::object::Rank& rank) {
    if (xuid.empty()) {
        throw std::invalid_argument("Parameter xuid must not be empty!");
    }

    power_ranks::manager::MainManager::setPlayerRankByXuid(xuid, rank);
}
