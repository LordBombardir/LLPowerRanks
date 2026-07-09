#include "Hooks.h"
#include "../commands/CommandManager.h"
#include "../config/ConfigManager.h"
#include "../core/MainManager.h"
#include "../rankFormats/RankFormatsManager.h"
#include "../utils/Utils.h"

#include "../Main.h"

#include <ll/api/memory/Hook.h>
#include <mc/network/packet/TextPacket.h>
#include <mc/network/packet/TextPacketPayload.h>
#include <mc/server/ServerPlayer.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandOutput.h>

namespace power_ranks {

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

    if (!CommandManager::isCommandAvailable(commandOrigin, mFlags, mPermissionLevel)) {
        const types::Rank& rank = MainManager::getPlayerRankOrSetDefault(player);
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

LL_TYPE_STATIC_HOOK(
    CreateChatHook,
    static_cast<HookPriority>(50),
    TextPacket,
    &TextPacket::createChat,
    TextPacket,
    const std::string&         author,
    const std::string&         message,
    std::optional<std::string> filteredMessage,
    const std::string&         xuid,
    const std::string&         platformId
) {
    Main::getInstance().getSelf().getLogger().info("Author: {}, message: \"{}\"", author, message);

    if (author.empty()) {
        return origin(author, message, filteredMessage, xuid, platformId);
    }

    if (author.find("§") != std::string::npos) {
        return origin("", message, filteredMessage, xuid, platformId);
    }

    const types::Rank& rank = MainManager::getPlayerRankOrSetDefault(author);

    std::string cleanMessage = message;
    if (!ConfigManager::getConfig().ranksWithColoredMessages.contains(rank.getName())) {
        cleanMessage = Utils::strTrim(Utils::clean(cleanMessage));
    }

    if (cleanMessage.empty()) {
        return {};
    }

    cleanMessage = RankFormatsManager::getChatFormat(rank.getName(), author, cleanMessage);
    return origin("", cleanMessage, filteredMessage, xuid, platformId);
}

void Hooks::setup() {
    CommandRunHook::hook();

    CreateChatHook::hook();
}

} // namespace power_ranks