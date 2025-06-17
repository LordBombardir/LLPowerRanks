#pragma once

#include "pool/ConnectionPool.h"
#include <ll/api/mod/NativeMod.h>
#include <mc/platform/UUID.h>
#include <memory>
#include <optional>
#include <string>

namespace power_ranks::manager {

class BaseManager final {
public:
    static void init(ll::mod::NativeMod& mod);
    static void dispose();

    std::optional<std::string> getPlayerRank(const mce::UUID& uuid);
    bool setPlayerRank(const mce::UUID& uuid, const std::string& rankName);

    static BaseManager* getInstance();

private:
    BaseManager(ll::mod::NativeMod& mod);
    ~BaseManager() = default;

    static std::unique_ptr<base::pool::ConnectionPool> connectionPool;
    static BaseManager*                                instance;
};

} // namespace power_ranks::manager