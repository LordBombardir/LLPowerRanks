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

    POWERRANKS_API Player& getPlayer() const;
    POWERRANKS_API const object::Rank& getPlayerRank() const;

    POWERRANKS_API std::string& getChatFormat() const;
    POWERRANKS_API std::string& getOriginalMessage() const;

private:
    Player&             player;
    const object::Rank& playerRank;
    std::string&        chatFormat;
    std::string&        originalMessage;
};

} // namespace power_ranks::object