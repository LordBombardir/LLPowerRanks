#pragma once

#include "types/Config.h"

#include <ll/api/mod/NativeMod.h>

namespace power_ranks {

class ConfigManager final {
public:
    static bool                  init(ll::mod::NativeMod& mod);
    static const config::Config& getConfig();

private:
    static config::Config config;
};

} // namespace power_ranks
