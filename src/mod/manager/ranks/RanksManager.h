#pragma once

#include "../../object/Rank.h"
#include "../config/ConfigManager.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/reflection/Deserialization.h>
#include <ll/api/reflection/Serialization.h>
#include <string>
#include <unordered_map>

namespace power_ranks::manager {

class RanksManager final {
public:
    struct Rank {
        std::string              prefix;
        std::string              chat;
        std::string              scoreTag;
        std::string              inheritanceRank   = "null";
        std::vector<std::string> availableCommands = {};
    };

    // clang-format off
    struct Config {
        int version = 1;
        std::unordered_map<std::string, Rank> ranks = {
            {ConfigManager::getConfig().defaultRankName, {"Player", "{prefix} {playerName}: {message}", "{prefix}"}},
            {"Vip", {"V.I.P.", "{prefix} {playerName}: §b{message}", "{prefix}", ConfigManager::getConfig().defaultRankName, {"home", "warp"}}},
            {"Creative", {"Creator", "{prefix} {playerName}: §a{message}", "{prefix}", "Vip", {"gamemode"}}},
            {"Helper", {"Mini-boss", "{prefix} {playerName}: §l{message}", "{prefix}", "Creative", {"kick", "mute"}}},
            {"Administrator", {"BOSS", "{prefix} {playerName}: §c§l{message}", "{prefix}", "Helper", {"ban"}}},
        };
    };
    // clang-format on

    static bool init(ll::mod::NativeMod& mod);
    static void dispose();

    static std::unordered_map<std::string, object::Rank*> getRanks();
    static std::optional<object::Rank*>                   getRank(const std::string& name);

    static void addRank(
        const std::string&                name,
        const std::string&                prefix,
        const std::string&                chatFormat,
        const std::string&                scoreTagFormat,
        const std::optional<const object::Rank*>& inheritanceRank = std::nullopt
    );
    static void removeRank(const object::Rank& rank);
    static void saveChangesRank(const object::Rank& rank);

private:
    static int  currentPriority;
    static void parseRanks();

    static std::string pathToConfig;
    static Config      config;

    static std::unordered_map<std::string, object::Rank*> ranks;
};

} // namespace power_ranks::manager