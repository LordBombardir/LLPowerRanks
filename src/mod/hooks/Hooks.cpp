#include "Hooks.h"
#include "../Utils.hpp"
#include "../manager/MainManager.h"
#include "../manager/command/CommandManager.h"
#include <ll/api/memory/Hook.h>
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
    HookPriority::Lowest,
    ServerNetworkHandler,
    &ServerNetworkHandler::$handle,
    void,
    const NetworkIdentifier& identifier,
    const TextPacket&        packet
) {
    if (ServerPlayer* player = thisFor<NetEventCallback>()->_getServerPlayer(identifier, packet.mClientSubId); player) {
        const object::Rank& rank        = manager::MainManager::getPlayerRankOrSetDefault(*player);
        TextPacket          otherPacket = TextPacket::createRawMessage(Utils::strReplace(
            rank.getChatFormat(),
            {"{prefix}", "{playerName}", "{message}"},
            {rank.getPrefix(), player->getRealName(), packet.mMessage}
        ));

        /*
         * Не использовать origin(identifier, packet);
         * По-видимому, ServerNetworkHandler::handle не хочет обрабатывать сырые TextPacket,
         * созданные при помощи TextPacket::createRawMessage
         * Так как этот хук отвечает лишь за оформление сообщения чата, то приоритет
         * HookPriority::Lowest является нормальным для функционирования других модов
         * (например, на блокировку чата).
         */

        player->getLevel().getPacketSender()->sendBroadcast(otherPacket);
        return;
    }

    origin(identifier, packet);
}

void Hooks::setupHooks() {
    PlayerConnectHook::hook();
    CommandRegistryAddEnumValueConstraintsHook::hook();
    CommandRunHook::hook();

    PlayerSendMessageHook::hook();
}

} // namespace power_ranks::hooks