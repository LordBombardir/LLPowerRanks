#pragma once

#include "../config/ConfigManager.h"
#include <ll/api/mod/NativeMod.h>
#include <ll/api/reflection/Deserialization.h>
#include <ll/api/reflection/Serialization.h>
#include <nlohmann/json.hpp>
#include <string>

namespace power_ranks::manager {

class RankFormatsManager final {
public:
    static bool init(ll::mod::NativeMod& mod);

    static bool isKnownLocaleCode(std::string_view localeCode, bool supportAll = false);

    static std::string getRawPrefixFormat(const std::string& rankName, const std::string& localeCode);
    static std::string getRawChatFormat(const std::string& rankName, const std::string& localeCode);
    static std::string getRawScoreTagFormat(const std::string& rankName, const std::string& localeCode);

    static std::string getPrefixFormat(const std::string& rankName);
    static std::string getScoreTagFormat(const std::string& rankName);

    static std::string
    getChatFormat(const std::string& rankName, const std::string& playerName, const std::string& message);

    static void setRankFormat(
        const std::string& rankName,
        const std::string& prefix,
        const std::string& chat,
        const std::string& scoreTag,
        std::string_view   localeCode = ConfigManager::getConfig().defaultLocaleCode
    );

    static void removeRankFormat(const std::string& rankName);

private:
    struct RankFormat {
        std::string prefix;
        std::string chat;
        std::string scoreTag;
    };

    // clang-format off
    struct Config {
        int version = 1;
        nlohmann::ordered_map<std::string, RankFormat> ranks = {
            {ConfigManager::getConfig().defaultRankName, {"Player", "{prefix} {playerName}: {message}", "{prefix}"}},
            {"Vip", {"V.I.P.", "{prefix} {playerName}: §b{message}", "{prefix}"}},
            {"Creative", {"Creator", "{prefix} {playerName}: §a{message}", "{prefix}"}},
            {"Helper", {"Mini-boss", "{prefix} {playerName}: §l{message}", "{prefix}"}},
            {"Administrator", {"BOSS", "{prefix} {playerName}: §c§l{message}", "{prefix}"}},
        };
    };
    // clang-format on

    struct ConfigInfo {
        std::filesystem::path pathToConfig;
        Config                config;
    };

    static std::unordered_map<std::string, ConfigInfo> configs;

    static void generatePlaceholders();

    static ConfigInfo& getConfig(std::string_view localeCode = ConfigManager::getConfig().defaultLocaleCode);
};

} // namespace power_ranks::manager