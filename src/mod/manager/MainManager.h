#pragma once

#include "../object/Rank.h"
#include <ll/api/mod/NativeMod.h>
#include <mc/network/packet/AvailableCommandsPacket.h>
#include <mc/world/actor/player/Player.h>

namespace power_ranks::manager {

class MainManager final {
public:
    static bool initManagers(ll::mod::NativeMod& mod);
    static void disposeManagers();

    static const object::Rank& getPlayerRankOrSetDefault(Player& player);
    static const object::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

    static void setPlayerRank(Player& player, const object::Rank& rank);
    static void setPlayerRankByName(const std::string& playerName, const object::Rank& rank);
    static void setPlayerRankByXuid(const std::string& xuid, const object::Rank& rank);

    static void updatePlayerRank(Player& player);

private:
    static AvailableCommandsPacket getAvailableCommandsPacket(const object::Rank& rank, Player& player);
};

} // namespace power_ranks::manager