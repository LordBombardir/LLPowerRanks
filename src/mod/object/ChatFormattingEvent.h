#pragma once

#include "Rank.h"
#include <ll/api/event/Event.h>
#include <mc/world/actor/player/Player.h>

#ifndef POWERRANKS_API
#ifdef POWERRANKS_EXPORT
#define POWERRANKS_API __declspec(dllexport)
#else
#define POWERRANKS_API __declspec(dllimport)
#endif
#endif

#ifndef POWERRANKS_NDAPI
#define POWERRANKS_NDAPI [[nodiscard]] POWERRANKS_API
#endif

namespace power_ranks::object {

class ChatFormattingEvent final : public ll::event::Event {
public:
    constexpr explicit ChatFormattingEvent(
        Player&             player,
        const object::Rank& playerRank,
        std::string&        chatFormat,
        std::string&        originalMessage
    )
    : player(player),
      playerRank(playerRank),
      chatFormat(chatFormat),
      originalMessage(originalMessage){};

    POWERRANKS_API void serialize(CompoundTag& nbt) const override;

    POWERRANKS_NDAPI Player& getPlayer() const;
    POWERRANKS_NDAPI const object::Rank& getPlayerRank() const;

    POWERRANKS_NDAPI std::string& getChatFormat() const;
    POWERRANKS_NDAPI std::string& getOriginalMessage() const;

protected:
    Player&             player;
    const object::Rank& playerRank;
    std::string&        chatFormat;
    std::string&        originalMessage;
};

} // namespace power_ranks::object