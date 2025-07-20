#pragma once

#include "Rank.h"
#include <ll/api/event/Event.h>
#include <mc/world/actor/player/Player.h>
#include <mc/world/events/ChatEvent.h>

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

namespace power_ranks::types {

class ChatFormattingEvent final : public ll::event::Event {
public:
    constexpr explicit ChatFormattingEvent(
        Player&      sender,
        const Rank&  senderRank,
        std::string& chatFormat,
        ChatEvent&   chatEvent
    )
    : sender(sender),
      senderRank(senderRank),
      chatFormat(chatFormat),
      chatEvent(chatEvent) {};

    POWERRANKS_API void serialize(CompoundTag& nbt) const override;

    POWERRANKS_NDAPI Player&     getSender() const;
    POWERRANKS_NDAPI const Rank& getSenderRank() const;

    POWERRANKS_NDAPI std::string& getChatFormat() const;
    POWERRANKS_NDAPI ChatEvent&   getChatEvent() const;

protected:
    Player&      sender;
    const Rank&  senderRank;
    std::string& chatFormat;
    ChatEvent&   chatEvent;
};

} // namespace power_ranks::types