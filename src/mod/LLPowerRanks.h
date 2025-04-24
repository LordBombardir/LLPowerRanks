#pragma once

#include "object/Rank.h"
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

POWERRANKS_API std::unordered_map<std::string, const object::Rank*> getRanks();
POWERRANKS_API std::optional<const object::Rank*> getRank(const std::string& name);

POWERRANKS_API const object::Rank& getPlayerRankOrSetDefault(Player& player);
POWERRANKS_API const object::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

POWERRANKS_API void setPlayerRank(Player& player, const object::Rank& rank);
POWERRANKS_API void setPlayerRankByName(const std::string& playerName, const object::Rank& rank);
POWERRANKS_API void setPlayerRankByXuid(const std::string& xuid, const object::Rank& rank);

} // namespace power_ranks::api
}