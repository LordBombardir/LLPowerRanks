#pragma once

#include "../types/Rank.h"
#include <ll/api/mod/NativeMod.h>
#include <mc/network/packet/AvailableCommandsPacket.h>
#include <mc/world/actor/player/Player.h>

namespace power_ranks::manager {

class MainManager final {
public:
    static bool initManagers(ll::mod::NativeMod& mod);
    static void disposeManagers();

    static const types::Rank& getPlayerRankOrSetDefault(Player& player);
    static const types::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

    static void setPlayerRank(Player& player, const types::Rank& rank);
    static void setPlayerRankByName(const std::string& playerName, const types::Rank& rank);
    static void setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank);

    static void updatePlayerRank(Player& player);

private:
    static void setScoreTag(Player& player, const std::string& scoreTag);

    static void extraActions(const types::Rank& rank, const Player& player);
    static void extraVanillaActions(Player& player, const types::Rank& rank);
};

} // namespace power_ranks::manager