#pragma once

#include "types/Rank.h"

#include <ll/api/event/Listener.h>
#include <ll/api/mod/NativeMod.h>
#include <mc/world/actor/player/Player.h>

namespace power_ranks {

class MainManager final {
public:
    static bool initModWhileLoading(ll::mod::NativeMod& mod);
    static bool initModWhileEnabling(ll::mod::NativeMod& mod);

    static void disableMod();

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

    static ll::event::ListenerPtr mCommandRegisterListener;
    static ll::event::ListenerPtr mPlayerJoinListener;
};

} // namespace power_ranks
