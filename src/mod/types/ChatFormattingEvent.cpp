#include "ChatFormattingEvent.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace power_ranks::types {

void ChatFormattingEvent::serialize(CompoundTag& nbt) const {
    ll::event::Event::serialize(nbt);
    nbt["sender"]     = ll::event::serializeRefObj(getSender());
    nbt["senderRank"] = ll::event::serializeRefObj(getSenderRank());
    nbt["chatFormat"] = getChatFormat();
    nbt["chatEvent"]  = ll::event::serializeRefObj(getChatEvent());
}

Player& ChatFormattingEvent::getSender() const { return sender; }

const Rank& ChatFormattingEvent::getSenderRank() const { return senderRank; }

std::string& ChatFormattingEvent::getChatFormat() const { return chatFormat; }

ChatEvent& ChatFormattingEvent::getChatEvent() const { return chatEvent; }

} // namespace power_ranks::types