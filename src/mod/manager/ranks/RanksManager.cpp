#include "RanksManager.h"
#include "../../Utils.hpp"
#include "../config/ConfigManager.h"
#include <ll/api/Config.h>
#include <stdexcept>

namespace power_ranks::manager {

int                                            RanksManager::currentPriority = 0;
RanksManager::Config                           RanksManager::config;
std::string                                    RanksManager::pathToConfig = "";
std::unordered_map<std::string, object::Rank*> RanksManager::ranks        = {};

bool RanksManager::init(ll::mod::NativeMod& mod) {
    pathToConfig = Utils::fixPath(mod.getDataDir().string() + "/ranks.json");

    try {
        bool result = ll::config::loadConfig(config, pathToConfig);
        parseRanks();

        return result;
    } catch (...) {}

    try {
        bool result = ll::config::saveConfig(config, pathToConfig);
        parseRanks();

        return result;
    } catch (...) {
        return false;
    }

    return false;
}

void RanksManager::dispose() {
    for (std::pair<std::string, object::Rank*> pair : ranks) {
        delete pair.second;
    }

    ranks.clear();
}

std::unordered_map<std::string, object::Rank*> RanksManager::getRanks() { return ranks; }
std::optional<object::Rank*>                   RanksManager::getRank(const std::string& name) {
    if (!ranks.contains(name)) {
        return std::nullopt;
    }

    return ranks[name];
}

void RanksManager::addRank(
    const std::string&                        name,
    const std::string&                        prefix,
    const std::string&                        chatFormat,
    const std::string&                        scoreTagFormat,
    const std::optional<const object::Rank*>& inheritanceRank
) {
    config.ranks[name] = Rank{
        prefix,
        chatFormat,
        scoreTagFormat,
        inheritanceRank.has_value() ? inheritanceRank.value()->getName() : "null"
    };

    ranks[name] = new object::Rank(currentPriority++, name, prefix, chatFormat, scoreTagFormat, inheritanceRank);
    ll::config::saveConfig(config, pathToConfig);
}

void RanksManager::removeRank(const object::Rank& rank) {
    config.ranks.erase(rank.getName());
    ll::config::saveConfig(config, pathToConfig);

    delete ranks[rank.getName()];
}

void RanksManager::saveChangesRank(const object::Rank& rank) {
    config.ranks[rank.getName()] = Rank{
        rank.getPrefix(),
        rank.getChatFormat(),
        rank.getScoreTagFormat(),
        rank.getInheritanceRank().has_value() ? rank.getInheritanceRank().value()->getName() : "null",
        rank.getAvailableCommands()
    };
    ll::config::saveConfig(config, pathToConfig);
}

void RanksManager::parseRanks() {
    if (!config.ranks.contains(ConfigManager::getConfig().defaultRankName)) {
        throw std::runtime_error("No default rank detected!");
    }

    for (std::pair<std::string, Rank> pair : config.ranks) {
        object::Rank* rank = new object::Rank(
            currentPriority++,
            pair.first,
            pair.second.prefix,
            pair.second.chat,
            pair.second.scoreTag,
            std::nullopt,
            pair.second.availableCommands
        );
        ranks[pair.first] = rank;
    }

    for (std::pair<std::string, Rank> pair : config.ranks) {
        if (pair.second.inheritanceRank != "null") {
            ranks[pair.first]->setInheritanceRank(ranks[pair.second.inheritanceRank]);
        }
    }
}

} // namespace power_ranks::manager