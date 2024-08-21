#include "ConfigManager.h"
#include "../../Utils.hpp"
#include <ll/api/Config.h>

namespace power_ranks::manager {

ConfigManager::MainConfig ConfigManager::config;

bool ConfigManager::init(ll::mod::NativeMod& mod) {
    std::string pathToConfig = Utils::fixPath(mod.getDataDir().string() + "/config.json");

    try {
        return ll::config::loadConfig(config, pathToConfig);
    } catch (...) {}

    try {
        return ll::config::saveConfig(config, pathToConfig);
    } catch (...) {
        return false;
    }

    return false;
}

const ConfigManager::MainConfig& ConfigManager::getConfig() { return config; }

} // namespace power_ranks::manager