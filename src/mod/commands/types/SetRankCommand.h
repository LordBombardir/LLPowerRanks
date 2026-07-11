#pragma once

#include "commands/CommandManager.h"
#include "lang/LanguageManager.h"

#include <ll/api/command/SoftEnum.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/CommandSelector.h>

namespace power_ranks::commands {

class SetRankCommand {
public:
    struct Parameter {
        ll::command::SoftEnum<CommandManager::RankNames> rankName;
        CommandSelector<Player>                          player;
    };

    static std::string getName() { return "setrank"; };
    static std::string getDescription() { return LanguageManager::getTranslate("commandSetRankDescription"); };
    static CommandPermissionLevel getRequirement() { return CommandPermissionLevel::GameDirectors; };
    static CommandFlag            getFlag() { return CommandFlagValue::NotCheat; };

    static std::vector<std::string> getAliases() { return {"set-rank"}; };

    static void execute(
        const CommandOrigin&            origin,
        CommandOutput&                  output,
        const Parameter&                parameter,
        [[maybe_unused]] const Command& command
    );
    static void executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output);
};

} // namespace power_ranks::commands
