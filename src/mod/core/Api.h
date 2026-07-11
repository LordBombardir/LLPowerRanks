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

/**
 * @brief Retrieves all registered ranks in the mod.
 * @return A map of rank names to const Rank pointers.
 */
POWERRANKS_API std::unordered_map<std::string, const types::Rank*> getRanks();

/**
 * @brief Retrieves a specific rank by its unique name.
 * @param name The name of the rank to find.
 * @return An optional containing a pointer to the Rank if found, or std::nullopt.
 */
POWERRANKS_API std::optional<const types::Rank*> getRank(const std::string& name);

/**
 * @brief Gets the current rank of an active player, setting it to the default rank if none is assigned.
 * @param player The player whose rank to retrieve.
 * @return A reference to the player's Rank object.
 */
POWERRANKS_API const types::Rank& getPlayerRankOrSetDefault(Player& player);

/**
 * @brief Gets the current rank of a player by name, setting it to the default rank if none is assigned.
 * @param playerName The real name of the player.
 * @return A reference to the player's Rank object.
 */
POWERRANKS_API const types::Rank& getPlayerRankOrSetDefault(const std::string& playerName);

/**
 * @brief Sets the rank of an active player.
 * @param player The active player object.
 * @param rank The Rank to assign to the player.
 */
POWERRANKS_API void setPlayerRank(Player& player, const types::Rank& rank);

/**
 * @brief Sets the rank of a player by their name.
 * @param playerName The real name of the player.
 * @param rank The Rank to assign to the player.
 */
POWERRANKS_API void setPlayerRankByName(const std::string& playerName, const types::Rank& rank);

/**
 * @brief Sets the rank of a player by their XUID.
 * @param xuid The XUID of the player.
 * @param rank The Rank to assign to the player.
 */
POWERRANKS_API void setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank);

} // namespace power_ranks::api
}
