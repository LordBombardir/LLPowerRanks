#include "RanksManager.h"
#include "../commands/CommandManager.h"
#include "../rankFormats/RankFormatsManager.h"

#include <ll/api/Config.h>
#include <stdexcept>

namespace power_ranks {

int                                                           RanksManager::currentPriority = 0;
RanksManager::Config                                          RanksManager::config;
std::filesystem::path                                         RanksManager::pathToConfig;
std::unordered_map<std::string, std::unique_ptr<types::Rank>> RanksManager::ranks = {};

bool RanksManager::init(ll::mod::NativeMod& mod) {
    pathToConfig = mod.getDataDir() / "ranks.json";

    try {
        bool result = ll::config::loadConfig(config, pathToConfig);
        parseRanks();

        return result;
    } catch (const std::exception& e) {
        mod.getLogger().error("Failed to load rank config: {}", e.what());
    }

    try {
        bool result = ll::config::saveConfig(config, pathToConfig);
        parseRanks();

        return result;
    } catch (const std::exception& e) {
        mod.getLogger().error("Failed to save rank config: {}", e.what());
        return false;
    }
}

void RanksManager::dispose() { ranks.clear(); }

nlohmann::ordered_map<std::string, types::Rank*> RanksManager::getOrderedRanks() {
    nlohmann::ordered_map<std::string, types::Rank*> result = {};
    for (const auto& [name, rawRank] : config.ranks) {
        const auto rank = getRank(name);
        if (rank.has_value()) {
            result[name] = *rank;
        }
    }

    return result;
}

std::unordered_map<std::string, types::Rank*> RanksManager::getRanks() {
    std::unordered_map<std::string, types::Rank*> result;
    for (const auto& [name, rankPtr] : ranks) {
        result[name] = rankPtr.get();
    }
    return result;
}

std::optional<types::Rank*> RanksManager::getRank(const std::string& name) {
    auto it = ranks.find(name);
    if (it == ranks.end()) {
        return std::nullopt;
    }

    return it->second.get();
}

void RanksManager::addRank(
    const std::string&                       name,
    const std::string&                       prefix,
    const std::string&                       chatFormat,
    const std::string&                       scoreTagFormat,
    const std::optional<const types::Rank*>& inheritanceRank
) {
    RankFormatsManager::setRankFormat(name, prefix, chatFormat, scoreTagFormat, "ALL");

    config.ranks[name] = Rank{inheritanceRank.has_value() ? inheritanceRank.value()->getName() : "null"};

    ranks[name] = std::make_unique<types::Rank>(currentPriority++, name, inheritanceRank);
    ll::config::saveConfig(config, pathToConfig);

    CommandManager::addRankNameToSoftEnum(name);
}

void RanksManager::removeRank(const types::Rank& rank) {
    std::string rankName = rank.getName();
    RankFormatsManager::removeRankFormat(rankName);

    config.ranks.erase(rankName);
    ll::config::saveConfig(config, pathToConfig);

    ranks.erase(rankName);

    CommandManager::removeRankNameFromSoftEnum(rankName);
}

void RanksManager::saveChangesRank(const types::Rank& rank) {
    config.ranks[rank.getName()] = Rank{
        rank.getInheritanceRank().has_value() ? rank.getInheritanceRank().value()->getName() : "null",
        rank.getAvailableCommands(),
        rank.getHiddenCommandOverloads().getData()
    };
    ll::config::saveConfig(config, pathToConfig);
}

void RanksManager::parseRanks() {
    if (config.ranks.find(ConfigManager::getConfig().defaultRankName) == config.ranks.end()) {
        throw std::runtime_error("No default rank detected!");
    }

    for (const auto& [name, rawRank] : config.ranks) {
        auto rank = std::make_unique<types::Rank>(
            currentPriority++,
            name,
            std::nullopt,
            rawRank.availableCommands,
            types::HiddenCommandOverloads(rawRank.hiddenCommandOverloads)
        );
        ranks[name] = std::move(rank);
    }

    for (const auto& [name, rawRank] : config.ranks) {
        if (rawRank.inheritanceRank != "null") {
            ranks[name]->setInheritanceRank(ranks[rawRank.inheritanceRank].get());
        }
    }
}

} // namespace power_ranks
