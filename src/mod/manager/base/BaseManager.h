#pragma once

#include "pool/ConnectionPool.h"
#include <ll/api/mod/NativeMod.h>
#include <memory>
#include <optional>
#include <string>

namespace power_ranks::manager {

class BaseManager final {
public:
    static void init(ll::mod::NativeMod& mod);
    static void dispose();

    std::optional<std::string> getPlayerRankByName(const std::string& playerName);
    std::optional<std::string> getPlayerRankByXuid(const std::string& xuid);

    bool setPlayerRank(
        const std::string& playerName,
        const std::string& xuid,
        const std::string& rankName
    );

    bool updatePlayerNameByXuid(const std::string& xuid, const std::string& playerName);
    bool updateXuidByPlayerName(const std::string& playerName, const std::string& xuid);
    bool updateRankNameByPlayerName(const std::string& playerName, const std::string& rankName);
    bool updateRankNameByXuid(const std::string& xuid, const std::string& rankName);

    static BaseManager* getInstance();

private:
    BaseManager(ll::mod::NativeMod& mod);
    ~BaseManager() = default;
    
    static std::unique_ptr<base::pool::ConnectionPool> connectionPool;
    static BaseManager*                                instance;
};

} // namespace power_ranks::manager