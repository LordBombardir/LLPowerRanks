#pragma once

#include <ll/api/data/KeyValueDB.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/platform/UUID.h>
#include <optional>
#include <string>

namespace power_ranks::manager {

class BaseManager final {
public:
    static void init(ll::mod::NativeMod& mod);

    static std::optional<std::string> getPlayerRank(const mce::UUID& uuid);
    static bool setPlayerRank(const mce::UUID& uuid, const std::string& rankName);

private:
    static std::unique_ptr<ll::data::KeyValueDB> database;
};

} // namespace power_ranks::manager