#include "ConfigManager.h"
#include "types/Config.h"

#include <ll/api/Config.h>

namespace power_ranks {

config::Config ConfigManager::config = {};

bool ConfigManager::init(ll::mod::NativeMod& mod) {
    const auto& pathToConfig = mod.getDataDir() / "config.json";

    try {
        return ll::config::loadConfig(config, pathToConfig);
    } catch (const std::exception& e) {
        mod.getLogger().error("Failed to load config: {}", e.what());
    }

    try {
        return ll::config::saveConfig(config, pathToConfig);
    } catch (const std::exception& e) {
        mod.getLogger().error("Failed to save config: {}", e.what());
        return false;
    }
}

const config::Config& ConfigManager::getConfig() { return config; }

} // namespace power_ranks
