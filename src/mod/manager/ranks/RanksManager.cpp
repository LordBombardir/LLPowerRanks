#include "RanksManager.h"
#include "../command/CommandManager.h"
#include <ll/api/Config.h>
#include <stdexcept>

namespace power_ranks::manager {

int                                           RanksManager::currentPriority = 0;
RanksManager::Config                          RanksManager::config;
std::filesystem::path                         RanksManager::pathToConfig;
std::unordered_map<std::string, types::Rank*> RanksManager::ranks = {};

bool RanksManager::init(ll::mod::NativeMod& mod) {
    pathToConfig = mod.getDataDir() / "ranks.json";

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
}

void RanksManager::dispose() {
    for (const auto& [name, rank] : ranks) {
        delete rank;
    }

    ranks.clear();
}

std::unordered_map<std::string, types::Rank*> RanksManager::getRanks() { return ranks; }

std::optional<types::Rank*> RanksManager::getRank(const std::string& name) {
    if (!ranks.contains(name)) {
        return std::nullopt;
    }

    return ranks[name];
}

void RanksManager::addRank(
    const std::string&                       name,
    const std::string&                       prefix,
    const std::string&                       chatFormat,
    const std::string&                       scoreTagFormat,
    const std::optional<const types::Rank*>& inheritanceRank
) {
    config.ranks[name] = Rank{
        prefix,
        chatFormat,
        scoreTagFormat,
        inheritanceRank.has_value() ? inheritanceRank.value()->getName() : "null"
    };

    ranks[name] = new types::Rank(currentPriority++, name, prefix, chatFormat, scoreTagFormat, inheritanceRank);
    ll::config::saveConfig(config, pathToConfig);

    manager::CommandManager::addRankNameToSoftEnum(name);
}

void RanksManager::removeRank(const types::Rank& rank) {
    std::string rankName = rank.getName();

    config.ranks.erase(rankName);
    ll::config::saveConfig(config, pathToConfig);

    delete ranks[rankName];
    ranks.erase(rankName);

    manager::CommandManager::removeRankNameFromSoftEnum(rankName);
}

void RanksManager::saveChangesRank(const types::Rank& rank) {
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
    if (config.ranks.find(ConfigManager::getConfig().defaultRankName) == config.ranks.end()) {
        throw std::runtime_error("No default rank detected!");
    }

    for (const auto& [name, rawRank] : config.ranks) {
        types::Rank* rank = new types::Rank(
            currentPriority++,
            name,
            rawRank.prefix,
            rawRank.chat,
            rawRank.scoreTag,
            std::nullopt,
            rawRank.availableCommands
        );
        ranks[name] = rank;
    }

    for (const auto& [name, rawRank] : config.ranks) {
        if (rawRank.inheritanceRank != "null") {
            ranks[name]->setInheritanceRank(ranks[rawRank.inheritanceRank]);
        }
    }
}

} // namespace power_ranks::manager