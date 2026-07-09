#include "RankFormatsManager.h"
#include "../config/types/Config.h"
#include "../utils/Utils.h"

#include <ll/api/Config.h>
#include <mc/deps/crypto/hash/Hash.h>
#include <mc/platform/UUID.h>
#include <placeholder_api/Api.h>

namespace power_ranks {

std::unordered_map<std::string, RankFormatsManager::ConfigInfo> RankFormatsManager::configs = {};

bool RankFormatsManager::init(ll::mod::NativeMod& mod) {
    bool result = false;

    std::filesystem::path pathToConfigs = mod.getDataDir() / "rankFormats";
    for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator(pathToConfigs)) {
        if (!entry.is_regular_file() || entry.path().extension() != ".json") {
            continue;
        }

        RankFormatsManager::Config config;

        try {
            result = ll::config::loadConfig(config, entry.path());
        } catch (const std::exception& e) {
            mod.getLogger().error("Failed to load rank format config from {}: {}", entry.path(), e.what());
            return false;
        }

        configs[entry.path().stem().string()] = {entry.path(), std::move(config)};
    }

    if (!configs.contains(ConfigManager::getConfig().defaultLocaleCode)) {
        mod.getLogger().error("Failed to find default locale config: {}", ConfigManager::getConfig().defaultLocaleCode);
        return false;
    }

    generatePlaceholders();
    return result;
}

bool RankFormatsManager::isKnownLocaleCode(std::string_view localeCode, bool supportAll) {
    return (localeCode == "ALL" && supportAll) || configs.find(std::string(localeCode)) != configs.end();
}

std::string RankFormatsManager::getRawPrefixFormat(const std::string& rankName, const std::string& localeCode) {
    const RankFormatsManager::ConfigInfo& configInfo = getConfig(localeCode);
    return configInfo.config.ranks.at(rankName).prefix;
}

std::string RankFormatsManager::getRawChatFormat(const std::string& rankName, const std::string& localeCode) {
    const RankFormatsManager::ConfigInfo& configInfo = getConfig(localeCode);
    return configInfo.config.ranks.at(rankName).chat;
}

std::string RankFormatsManager::getRawScoreTagFormat(const std::string& rankName, const std::string& localeCode) {
    const RankFormatsManager::ConfigInfo& configInfo = getConfig(localeCode);
    return configInfo.config.ranks.at(rankName).scoreTag;
}

std::string RankFormatsManager::getPrefixFormat(const std::string& rankName) {
    return placeholder::api::generatePlaceholder(std::format("PowerRanks_{}_prefix", rankName));
}

std::string RankFormatsManager::getScoreTagFormat(const std::string& rankName) {
    return placeholder::api::generatePlaceholder(std::format("PowerRanks_{}_scoreTag", rankName));
}

std::string RankFormatsManager::getChatFormat(
    const std::string& rankName,
    const std::string& playerName,
    const std::string& message
) {
    const auto& prefixFormat         = getPrefixFormat(rankName);
    const auto& temporaryPlaceholder = placeholder::api::generateTemporaryPlaceholder();

    for (const auto& [localeCode, configInfo] : configs) {
        const auto& rankFormat = configInfo.config.ranks.at(rankName);
        const auto& chat       = Utils::strReplace(
            rankFormat.chat,
            {"{prefix}", "{playerName}", "{message}"},
            {prefixFormat, playerName, message}
        );

        placeholder::api::setTemporaryPlaceholder(temporaryPlaceholder, chat, localeCode);
    }

    return temporaryPlaceholder;
}

void RankFormatsManager::setRankFormat(
    const std::string& rankName,
    const std::string& prefix,
    const std::string& chat,
    const std::string& scoreTag,
    std::string_view   localeCode
) {
    const auto& prefixPlaceholder   = getPrefixFormat(rankName);
    const auto& scoreTagPlaceholder = getScoreTagFormat(rankName);

    if (localeCode == "ALL") {
        for (auto& [configLocaleCode, configInfo] : configs) {
            configInfo.config.ranks[rankName].prefix   = prefix;
            configInfo.config.ranks[rankName].chat     = chat;
            configInfo.config.ranks[rankName].scoreTag = scoreTag;

            ll::config::saveConfig(configInfo.config, configInfo.pathToConfig);

            placeholder::api::setPlaceholder(prefixPlaceholder, prefix, configLocaleCode);
            placeholder::api::setPlaceholder(
                scoreTagPlaceholder,
                Utils::strReplace(scoreTag, "{prefix}", prefix),
                configLocaleCode
            );
        }
        return;
    }

    RankFormatsManager::ConfigInfo& configInfo = getConfig(localeCode);

    configInfo.config.ranks[rankName].prefix   = prefix;
    configInfo.config.ranks[rankName].chat     = chat;
    configInfo.config.ranks[rankName].scoreTag = scoreTag;

    ll::config::saveConfig(configInfo.config, configInfo.pathToConfig);

    placeholder::api::setPlaceholder(prefixPlaceholder, prefix, std::string(localeCode));
    placeholder::api::setPlaceholder(
        scoreTagPlaceholder,
        Utils::strReplace(scoreTag, "{prefix}", prefix),
        std::string(localeCode)
    );
}

void RankFormatsManager::removeRankFormat(const std::string& rankName) {
    for (auto& [localeCode, configInfo] : configs) {
        configInfo.config.ranks.erase(rankName);
        ll::config::saveConfig(configInfo.config, configInfo.pathToConfig);
    }
}

void RankFormatsManager::generatePlaceholders() {
    for (const auto& [localeCode, configInfo] : configs) {
        for (const auto& [rankName, rankFormat] : configInfo.config.ranks) {
            const auto& prefixPlaceholder   = getPrefixFormat(rankName);
            const auto& scoreTagPlaceholder = getScoreTagFormat(rankName);

            placeholder::api::setPlaceholder(prefixPlaceholder, rankFormat.prefix, localeCode);
            placeholder::api::setPlaceholder(
                scoreTagPlaceholder,
                Utils::strReplace(rankFormat.scoreTag, "{prefix}", rankFormat.prefix),
                localeCode
            );
        }
    }
}

RankFormatsManager::ConfigInfo& RankFormatsManager::getConfig(std::string_view localeCode) {
    auto it = configs.find(std::string(localeCode));
    if (it != configs.end()) {
        return it->second;
    }

    return configs.at(ConfigManager::getConfig().defaultLocaleCode);
}

} // namespace power_ranks
