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
#include <mc/server/commands/CommandRegistry.h>
#include <mc/world/level/Level.h>

namespace power_ranks::hooks {

LL_TYPE_INSTANCE_HOOK(
    ServerNetworkHandlerSendLoginMessageLocalHook,
    HookPriority::Normal,
    ServerNetworkHandler,
    &ServerNetworkHandler::sendLoginMessageLocal,
    void,
    const NetworkIdentifier& source,
    const ConnectionRequest& connectionRequest,
    ServerPlayer&            player
) {
    origin(source, connectionRequest, player);
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
    CommandRegistryCheckOriginCommandFlagsHook,
    HookPriority::Normal,
    CommandRegistry,
    &CommandRegistry::checkOriginCommandFlags,
    bool,
    const CommandOrigin&   commandOrigin,
    CommandFlag            flags,
    CommandPermissionLevel permissionLevel
) {
    if (commandOrigin.getEntity() == nullptr || !commandOrigin.getEntity()->isType(ActorType::Player)) {
        return origin(commandOrigin, flags, permissionLevel);
    }

    ServerPlayer& player = static_cast<ServerPlayer&>(*commandOrigin.getEntity());

    const object::Rank&        rank = manager::MainManager::getPlayerRankOrSetDefault(player);
    std::optional<std::string> lastWrittedCommand =
        manager::CommandManager::getAndRemoveLastWrittedCommand(player.getRealName());

    if (!lastWrittedCommand.has_value()) {
        return origin(commandOrigin, flags, permissionLevel);
    }

    if (rank.isCommandAvailable(lastWrittedCommand.value())) {
        return true;
    }

    return origin(commandOrigin, flags, permissionLevel);
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
    manager::CommandManager::setLastWrittedCommand(player.getRealName(), getCommandName());

    origin(commandOrigin, output);
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
    ServerNetworkHandlerSendLoginMessageLocalHook::hook();
    CommandRegistryAddEnumValueConstraintsHook::hook();
    CommandRegistryCheckOriginCommandFlagsHook::hook();
    CommandRunHook::hook();

    PlayerSendMessageHook::hook();
}

} // namespace power_ranks::hooks