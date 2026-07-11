#pragma once

#include "config/ConfigManager.h"
#include "types/Rank.h"

#include <ll/api/mod/NativeMod.h>
#include <ll/api/reflection/Deserialization.h>
#include <ll/api/reflection/Serialization.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace power_ranks {

class RanksManager final {
public:
    struct Rank {
        std::string                                              inheritanceRank        = "null";
        std::unordered_set<std::string>                          availableCommands      = {};
        std::unordered_map<std::string, std::unordered_set<int>> hiddenCommandOverloads = {};
        std::vector<std::string>                                 additionalInformation  = {};
    };

    // clang-format off
    struct Config {
        int version = 3;
        nlohmann::ordered_map<std::string, Rank> ranks = {
            {ConfigManager::getConfig().defaultRankName, {}},
            {"Vip", {ConfigManager::getConfig().defaultRankName, {"home", "warp"}}},
            {"Creative", {"Vip", {"gamemode"}}},
            {"Helper", {"Creative", {"kick", "mute"}}},
            {"Administrator", {"Helper", {"ban"}}},
        };
    };
    // clang-format on

    static bool init(ll::mod::NativeMod& mod);
    static void dispose();

    static nlohmann::ordered_map<std::string, types::Rank*> getOrderedRanks();

    static std::unordered_map<std::string, types::Rank*> getRanks();
    static std::optional<types::Rank*>                   getRank(const std::string& name);

    static void addRank(
        const std::string&                       name,
        const std::string&                       prefix,
        const std::string&                       chatFormat,
        const std::string&                       scoreTagFormat,
        const std::optional<const types::Rank*>& inheritanceRank = std::nullopt
    );
    static void removeRank(const types::Rank& rank);
    static void saveChangesRank(const types::Rank& rank);

private:
    static int  currentPriority;
    static void parseRanks();

    static std::filesystem::path pathToConfig;
    static Config                config;

    static std::unordered_map<std::string, std::unique_ptr<types::Rank>> ranks;
};

} // namespace power_ranks
