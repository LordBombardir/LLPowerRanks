#include "Hooks.h"
#include "../manager/MainManager.h"
#include "../manager/command/CommandManager.h"
#include "../manager/config/ConfigManager.h"
#include "../types/ChatFormattingEvent.h"
#include "../utils/Utils.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <mc/deps/ecs/WeakEntityRef.h>
#include <mc/deps/ecs/gamerefs_entity/EntityContext.h>
#include <mc/deps/game_refs/GameRefs.h>
#include <mc/deps/game_refs/WeakRef.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/SetLocalPlayerAsInitializedPacket.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandOutput.h>

namespace power_ranks::hooks {

LL_TYPE_INSTANCE_HOOK(
    PlayerJoinHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    const NetworkIdentifier&                 identifier,
    const SetLocalPlayerAsInitializedPacket& packet
) {
    if (ServerPlayer* player = thisFor<NetEventCallback>()->_getServerPlayer(identifier, packet.mSenderSubId); player) {
        manager::MainManager::updatePlayerRank(*player);
    }

    origin(identifier, packet);
}

LL_TYPE_INSTANCE_HOOK(
    CommandRunHook,
    HookPriority::Normal,
    Command,
    &Command::run,
    void,
    const CommandOrigin& commandOrigin,
    CommandOutput&       output
) {
    if (commandOrigin.getEntity() == nullptr || !commandOrigin.getEntity()->isPlayer()) {
        return origin(commandOrigin, output);
    }

    ServerPlayer& player = static_cast<ServerPlayer&>(*commandOrigin.getEntity());

    if (!manager::CommandManager::isCommandAvailable(commandOrigin, mFlags, mPermissionLevel)) {
        const types::Rank& rank = manager::MainManager::getPlayerRankOrSetDefault(player);
        if (!rank.isCommandAvailable(getCommandName())) {
            output.addMessage(
                "commands.generic.unknown",
                {CommandOutputParameter({getCommandName()})},
                CommandOutputMessageType::Error
            );
            return sendTelemetry(commandOrigin, output);
        }
    }

    // TODO: implement CommandRunStats...

    execute(commandOrigin, output);
    return sendTelemetry(commandOrigin, output);
}

LL_TYPE_INSTANCE_HOOK(
    DisplayGameMessageHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::_displayGameMessage,
    void,
    const Player& sender,
    ChatEvent&    chatEvent
) {
    Player& player = const_cast<Player&>(sender);

    const types::Rank& rank = manager::MainManager::getPlayerRankOrSetDefault(player);

    std::string chatFormat = rank.getChatFormat();
    ll::event::EventBus::getInstance().publish(types::ChatFormattingEvent{player, rank, chatFormat, chatEvent});

    if (!manager::ConfigManager::getConfig().ranksWithColoredMessages.contains(rank.getName())) {
        chatEvent.mMessage = Utils::strTrim(Utils::clean(chatEvent.mMessage));
    }

    if (chatEvent.mMessage->empty()) {
        return;
    }

    chatEvent.mMessage = Utils::strReplace(
        chatFormat,
        {"{prefix}", "{playerName}", "{message}"},
        {rank.getPrefix(), player.getRealName(), chatEvent.mMessage}
    );

    return origin(sender, chatEvent);
}

LL_TYPE_STATIC_HOOK(
    TextPacketCreateChatHook,
    HookPriority::Normal,
    TextPacket,
    &TextPacket::createChat,
    TextPacket,
    [[maybe_unused]] const std::string& author,
    const std::string&                  message,
    std::optional<std::string>          filteredMessage,
    const std::string&                  xuid,
    const std::string&                  platformId
) {
    return origin("", message, filteredMessage, xuid, platformId);
}

void setupHooks() {
    PlayerJoinHook::hook();
    CommandRunHook::hook();

    DisplayGameMessageHook::hook();
    TextPacketCreateChatHook::hook();
}

static std::unique_ptr<ll::event::EmitterBase> emitterFactory();
class DisplayGameMessageHookEmitter : public ll::event::Emitter<emitterFactory, types::ChatFormattingEvent> {
    ll::memory::HookRegistrar<DisplayGameMessageHook> hook;
};

static std::unique_ptr<ll::event::EmitterBase> emitterFactory() {
    return std::make_unique<DisplayGameMessageHookEmitter>();
}

} // namespace power_ranks::hooks