#include "ChatFormattingEvent.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace power_ranks::types {

void ChatFormattingEvent::serialize(CompoundTag& nbt) const {
    ll::event::Event::serialize(nbt);
    nbt["player"]          = ll::event::serializeRefObj(getPlayer());
    nbt["playerRank"]      = ll::event::serializeRefObj(getPlayerRank());
    nbt["chatFormat"]      = getChatFormat();
    nbt["originalMessage"] = getOriginalMessage();
}

Player& ChatFormattingEvent::getPlayer() const { return player; }

const Rank& ChatFormattingEvent::getPlayerRank() const { return playerRank; }

std::string& ChatFormattingEvent::getChatFormat() const { return chatFormat; }

std::string& ChatFormattingEvent::getOriginalMessage() const { return originalMessage; }

} // namespace power_ranks::types