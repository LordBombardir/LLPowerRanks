#pragma once

#include "object/Rank.h"
#include <mc/world/actor/player/Player.h>
#include <optional>

#ifdef POWERRANKS_EXPORT
#define POWERRANKS_API __declspec(dllexport)
#else
#define POWERRANKS_API __declspec(dllimport)
#endif

extern "C++" {

POWERRANKS_API std::unordered_map<std::string, const power_ranks::object::Rank*> getRanks();
POWERRANKS_API const std::optional<const power_ranks::object::Rank*> getRank(const std::string& name);

POWERRANKS_API const power_ranks::object::Rank& getPlayerRankOrSetDefault(Player& player);
POWERRANKS_API const power_ranks::object::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

POWERRANKS_API void setPlayerRank(Player& player, const power_ranks::object::Rank& rank);
POWERRANKS_API void setPlayerRankByName(const std::string& playerName, const power_ranks::object::Rank& rank);
POWERRANKS_API void setPlayerRankByXuid(const std::string& xuid, const power_ranks::object::Rank& rank);
}