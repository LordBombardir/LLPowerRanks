#include "ChatFormattingEvent.h"
#include <ll/api/event/EventRefObjSerializer.h>

namespace power_ranks::object {

void ChatFormattingEvent::serialize(CompoundTag& nbt) const {
    ll::event::Event::serialize(nbt);
    nbt["player"]          = ll::event::serializeRefObj(getPlayer());
    nbt["playerRank"]      = ll::event::serializeRefObj(getPlayerRank());
    nbt["chatFormat"]      = getChatFormat();
    nbt["originalMessage"] = getOriginalMessage();
}

} // namespace power_ranks::object