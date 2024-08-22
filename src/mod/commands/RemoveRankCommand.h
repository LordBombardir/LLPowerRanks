#pragma once

#include "../manager/command/CommandManager.h"
#include "../manager/lang/LanguageManager.h"
#include <fmt/std.h>
#include <ll/api/command/SoftEnum.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/world/level/Command.h>

namespace power_ranks::commands {

class RemoveRankCommand {
public:
    struct Parameter {
        ll::command::SoftEnum<manager::CommandManager::RankNames> rankName;
    };

    static std::string getName() { return "removerank"; };
    static std::string getDescription() {
        return manager::LanguageManager::getInstance()->getTranslate("commandRemoveRankDescription");
    };
    static CommandPermissionLevel getRequirement() { return CommandPermissionLevel::GameDirectors; };
    static CommandFlag            getFlag() { return CommandFlagValue::NotCheat; };

    static std::vector<std::string> getAliases() { return {"remove-rank"}; };

    static void
    execute(const CommandOrigin& origin, CommandOutput& output, const Parameter& parameter, const Command& command);
    static void executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output);
};

} // namespace power_ranks::commands