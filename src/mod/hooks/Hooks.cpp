#include "Hooks.h"
#include "../Main.h"
#include "../Utils.hpp"
#include "../manager/MainManager.h"
#include "../manager/command/CommandManager.h"
#include "../object/ChatFormattingEvent.h"
#include <ll/api/event/Emitter.h>
#include <ll/api/event/EventBus.h>
#include <ll/api/memory/Hook.h>
#include <mc/network/NetworkSystem.h>
#include <mc/network/PacketSender.h>
#include <mc/network/ServerNetworkHandler.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/world/level/Level.h>

namespace power_ranks::hooks {

LL_TYPE_INSTANCE_HOOK(
    PlayerConnectHook,
    HookPriority::Low,
    ServerNetworkHandler,
    &ServerNetworkHandler::sendLoginMessageLocal,
    void,
    const NetworkIdentifier& networkIdentifier,
    const ConnectionRequest& connectionRequest,
    ServerPlayer&            player
) {
    origin(networkIdentifier, connectionRequest, player);
    manager::MainManager::updatePlayerRank(player);
}

LL_TYPE_INSTANCE_HOOK(
    CommandRegistryAddEnumValueConstraintsHook,
    HookPriority::Normal,
    CommandRegistry,
    &CommandRegistry::addEnumValueConstraints,
    void,
    const std::string&              enumName,
    const std::vector<std::string>& enumValues,
    SemanticConstraint              constraint
) {
    constraint = static_cast<SemanticConstraint>(
        static_cast<uchar>(constraint) & ~static_cast<uchar>(SemanticConstraint::RequiresElevatedPermissions)
    );

    return origin(enumName, enumValues, constraint);
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
    if (commandOrigin.getEntity() == nullptr || !commandOrigin.getEntity()->isType(ActorType::Player)) {
        return origin(commandOrigin, output);
    }

    ServerPlayer& player = static_cast<ServerPlayer&>(*commandOrigin.getEntity());

    if (!manager::CommandManager::isCommandAvailable(commandOrigin, mFlags, mPermissionLevel)) {
        const object::Rank& rank = manager::MainManager::getPlayerRankOrSetDefault(player);
        if (!rank.isCommandAvailable(getCommandName())) {
            output.addMessage(
                "commands.generic.unknown",
                {CommandOutputParameter({getCommandName()})},
                CommandOutputMessageType::Error
            );
            return sendTelemetry(commandOrigin, output);
        }
    }

    execute(commandOrigin, output);
    return sendTelemetry(commandOrigin, output);
}

LL_TYPE_INSTANCE_HOOK(
    PlayerSendMessageHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    const NetworkIdentifier& identifier,
    const TextPacket&        packet
) {
    if (ServerPlayer* player = thisFor<NetEventCallback>()->_getServerPlayer(identifier, packet.mClientSubId); player) {
        const object::Rank& rank         = manager::MainManager::getPlayerRankOrSetDefault(*player);
        TextPacket&         castedPacket = const_cast<TextPacket&>(packet);

        std::string chatFormat = rank.getChatFormat();
        ll::event::EventBus::getInstance().publish(
            object::ChatFormattingEvent{*player, rank, chatFormat, castedPacket.mMessage}
        );

        castedPacket.mMessage = Utils::strReplace(
            chatFormat,
            {"{prefix}", "{playerName}", "{message}"},
            {rank.getPrefix(), player->getRealName(), castedPacket.mMessage}
        );

        return origin(identifier, castedPacket);
    }

    origin(identifier, packet);
}

LL_TYPE_INSTANCE_HOOK(
    NetworkSystemSendHook,
    HookPriority::Normal,
    NetworkSystem,
    &NetworkSystem::send,
    void,
    const NetworkIdentifier& identifier,
    const Packet&            packet,
    SubClientId              subId
) {
    if (packet.getId() == MinecraftPacketIds::Text) {
        TextPacket& castedPacket = const_cast<TextPacket&>(static_cast<const TextPacket&>(packet));
        if (castedPacket.mType != TextPacketType::Chat) {
            return origin(identifier, packet, subId);
        }

        castedPacket.mAuthor = "";
        return origin(identifier, castedPacket, subId);
    }

    origin(identifier, packet, subId);
}

LL_TYPE_INSTANCE_HOOK(
    NetworkSystemSendToMultipleHook,
    HookPriority::Normal,
    NetworkSystem,
    &NetworkSystem::sendToMultiple,
    void,
    const std::vector<NetworkIdentifierWithSubId>& identifiers,
    const Packet&                                  packet
) {
    if (packet.getId() == MinecraftPacketIds::Text) {
        TextPacket& castedPacket = const_cast<TextPacket&>(static_cast<const TextPacket&>(packet));
        if (castedPacket.mType != TextPacketType::Chat) {
            return origin(identifiers, packet);
        }

        castedPacket.mAuthor = "";
        return origin(identifiers, castedPacket);
    }

    origin(identifiers, packet);
}

void Hooks::setupHooks() {
    PlayerConnectHook::hook();
    CommandRegistryAddEnumValueConstraintsHook::hook();
    CommandRunHook::hook();

    PlayerSendMessageHook::hook();
    NetworkSystemSendHook::hook();
    NetworkSystemSendToMultipleHook::hook();
}

// static std::unique_ptr<ll::event::EmitterBase> emitterFactory();
// class PlayerSendMessageEmitter : public ll::event::Emitter<emitterFactory, object::ChatFormattingEvent> {
//     ll::memory::HookRegistrar<PlayerSendMessageHook> hook;
// };

// static std::unique_ptr<ll::event::EmitterBase> emitterFactory() { return std::make_unique<PlayerSendMessageEmitter>(); }

} // namespace power_ranks::hooks