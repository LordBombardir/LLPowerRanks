#pragma once

#include "../../lang/LanguageManager.h"
#include "../CommandManager.h"

#include <ll/api/command/SoftEnum.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>

namespace power_ranks::commands {

class EditRankCommand {
public:
    struct FirstParameter {
        ll::command::SoftEnum<CommandManager::RankNames> rankName;
        std::string                                      localeCode;
    };

    struct SecondParameter {
        ll::command::SoftEnum<CommandManager::RankNames> rankName;
        std::string                                      prefix;
        std::string                                      chat;
        std::string                                      scoreTag;
        std::string                                      localeCode;
        ll::command::SoftEnum<CommandManager::RankNames> inheritanceRank;
        std::string                                      availableCommands;
        std::string                                      hiddenCommandOverloads;
        std::string                                      additionalInformation;
    };

    static std::string getName() { return "editrank"; };
    static std::string getDescription() { return LanguageManager::getTranslate("commandEditRankDescription"); };
    static CommandPermissionLevel getRequirement() { return CommandPermissionLevel::GameDirectors; };
    static CommandFlag            getFlag() { return CommandFlagValue::NotCheat; };

    static std::vector<std::string> getAliases() { return {"edit-rank"}; };

    static void executeFirstParameter(
        const CommandOrigin&            origin,
        CommandOutput&                  output,
        const FirstParameter&           parameter,
        [[maybe_unused]] const Command& command
    );
    static void executeSecondParameter(
        const CommandOrigin&            origin,
        CommandOutput&                  output,
        const SecondParameter&          parameter,
        [[maybe_unused]] const Command& command
    );

    static void executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output);
};

} // namespace power_ranks::commands
