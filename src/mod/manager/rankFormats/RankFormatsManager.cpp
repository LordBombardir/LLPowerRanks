#include "RankFormatsManager.h"
#include "../../utils/Utils.h"
#include <ll/api/Config.h>
#include <mc/deps/crypto/hash/Hash.h>
#include <mc/platform/UUID.h>
#include <translator_api/Api.h>

namespace power_ranks::manager {

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
        } catch (...) {
            return false;
        }

        configs[entry.path().stem().string()] = {entry.path(), std::move(config)};
    }

    if (!configs.contains(ConfigManager::getConfig().defaultLocaleCode)) {
        return false;
    }

    return result;
}

std::string RankFormatsManager::getPrefixFormat(const std::string& rankName, std::string_view localeCode) {
    const RankFormatsManager::Config& config = getConfig(localeCode).config;

    auto it = config.ranks.find(rankName);
    if (it == config.ranks.end()) {
        return config.ranks.at(ConfigManager::getConfig().defaultRankName).prefix;
    }

    return it->second.prefix;
}

std::string RankFormatsManager::getChatFormat(const std::string& rankName, std::string_view localeCode) {
    // todo: сделать генерацию rankFormats при запуске мода. Это гораздо эффективнее и лучше смотрится.
}

std::string RankFormatsManager::getScoreTagFormat(const std::string& rankName, std::string_view localeCode) {
    const RankFormatsManager::Config& config = getConfig(localeCode).config;

    auto it = config.ranks.find(rankName);
    if (it == config.ranks.end()) {
        return config.ranks.at(ConfigManager::getConfig().defaultRankName).scoreTag;
    }

    return it->second.scoreTag;
}

void RankFormatsManager::setRankFormat(
    const std::string& rankName,
    const std::string& prefix,
    const std::string& chat,
    const std::string& scoreTag,
    std::string_view   localeCode
) {
    if (localeCode == "ALL") {
        for (auto& [configLocaleCode, configInfo] : configs) {
            configInfo.config.ranks[rankName].prefix   = prefix;
            configInfo.config.ranks[rankName].chat     = chat;
            configInfo.config.ranks[rankName].scoreTag = scoreTag;

            ll::config::saveConfig(configInfo.config, configInfo.pathToConfig);
        }
        return;
    }

    RankFormatsManager::ConfigInfo& configInfo = getConfig(localeCode);

    configInfo.config.ranks[rankName].prefix   = prefix;
    configInfo.config.ranks[rankName].chat     = chat;
    configInfo.config.ranks[rankName].scoreTag = scoreTag;

    ll::config::saveConfig(configInfo.config, configInfo.pathToConfig);
}

RankFormatsManager::ConfigInfo& RankFormatsManager::getConfig(std::string_view localeCode) {
    auto it = configs.find(std::string(localeCode));
    if (it != configs.end()) {
        return it->second;
    }

    return configs.at(ConfigManager::getConfig().defaultLocaleCode);
}

} // namespace power_ranks::manager