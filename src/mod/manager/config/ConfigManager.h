#pragma once

#include <ll/api/mod/NativeMod.h>
#include <string>
#include <vector>

namespace power_ranks::manager {

class ConfigManager final {
public:
    struct MainConfig {
        int                      version           = 2;
        std::string              defaultLocaleCode = "en_US";
        std::string              defaultRankName   = "Default";
        std::vector<std::string> superRanks        = {"Helper", "Administrator"};
        std::vector<std::string> superPlayers      = {"LordBombardir"};
    };

    static bool              init(ll::mod::NativeMod& mod);
    static const MainConfig& getConfig();

private:
    static MainConfig config;
};

} // namespace power_ranks::manager