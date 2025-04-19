#include "Hooks.h"
#include "../Utils.hpp"
#include "../manager/MainManager.h"
#include "../manager/command/CommandManager.h"
#include <ll/api/memory/Hook.h>
#include <mc/network/LoopbackPacketSender.h>
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

        castedPacket.mMessage = Utils::strReplace(
            rank.getChatFormat(),
            {"{prefix}", "{playerName}", "{message}"},
            {rank.getPrefix(), player->getRealName(), packet.mMessage}
        );

        return origin(identifier, castedPacket);
    }

    origin(identifier, packet);
}

LL_TYPE_INSTANCE_HOOK(
    LoopbackPacketSenderHook,
    HookPriority::Normal,
    LoopbackPacketSender,
    &LoopbackPacketSender::$sendToClient,
    void,
    const NetworkIdentifier& identifier,
    const Packet&            packet,
    SubClientId              subId
) {
    if (packet.getId() == MinecraftPacketIds::Text) {
        TextPacket& castedPacket = const_cast<TextPacket&>(static_cast<const TextPacket&>(packet));

        castedPacket.mAuthor = "";
        return origin(identifier, castedPacket, subId);
    }

    origin(identifier, packet, subId);
}

void Hooks::setupHooks() {
    PlayerConnectHook::hook();
    CommandRegistryAddEnumValueConstraintsHook::hook();
    CommandRunHook::hook();

    PlayerSendMessageHook::hook();
    LoopbackPacketSenderHook::hook();
}

} // namespace power_ranks::hooks