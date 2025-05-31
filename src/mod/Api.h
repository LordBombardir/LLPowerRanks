#pragma once

#include "types/Rank.h"
#include <mc/world/actor/player/Player.h>
#include <optional>

#ifndef POWERRANKS_API
#ifdef POWERRANKS_EXPORT
#define POWERRANKS_API __declspec(dllexport)
#else
#define POWERRANKS_API __declspec(dllimport)
#endif
#endif

extern "C++" {

namespace power_ranks::api {

POWERRANKS_API std::unordered_map<std::string, const types::Rank*> getRanks();
POWERRANKS_API std::optional<const types::Rank*> getRank(const std::string& name);

POWERRANKS_API const types::Rank& getPlayerRankOrSetDefault(Player& player);
POWERRANKS_API const types::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

POWERRANKS_API void setPlayerRank(Player& player, const types::Rank& rank);
POWERRANKS_API void setPlayerRankByName(const std::string& playerName, const types::Rank& rank);
POWERRANKS_API void setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank);

} // namespace power_ranks::api
}