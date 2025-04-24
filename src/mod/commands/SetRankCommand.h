#pragma once

#include "../manager/command/CommandManager.h"
#include "../manager/lang/LanguageManager.h"
#include <ll/api/command/SoftEnum.h>
#include <mc/server/commands/Command.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/CommandSelector.h>
#include <mc/world/actor/player/Player.h>

namespace power_ranks::commands {

class SetRankCommand {
public:
    struct Parameter {
        CommandSelector<Player>                                   player;
        ll::command::SoftEnum<manager::CommandManager::RankNames> rankName;
    };

    static std::string getName() { return "setrank"; };
    static std::string getDescription() { return manager::LanguageManager::getTranslate("commandSetRankDescription"); };
    static CommandPermissionLevel getRequirement() { return CommandPermissionLevel::GameDirectors; };
    static CommandFlag            getFlag() { return CommandFlagValue::NotCheat; };

    static std::vector<std::string> getAliases() { return {"set-rank"}; };

    static void execute(
        const CommandOrigin&                             origin,
        CommandOutput&                                   output,
        const Parameter&                                 parameter,
        [[maybe_unused]] const Command& command
    );
    static void executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output);
};

} // namespace power_ranks::commands