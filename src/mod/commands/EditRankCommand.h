#pragma once

#include "../manager/command/CommandManager.h"
#include "../manager/lang/LanguageManager.h"
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
        ll::command::SoftEnum<manager::CommandManager::RankNames> rankName;
        std::string                                               prefix;
        std::string                                               chatFormat;
        std::string                                               scoreTagFormat;
        ll::command::SoftEnum<manager::CommandManager::RankNames> inheritanceRank;
        std::string                                               availableCommands;
        std::string                                               hiddenCommandOverloads;
    };

    struct SecondParameter {
        ll::command::SoftEnum<manager::CommandManager::RankNames> rankName;
    };

    static std::string getName() { return "editrank"; };
    static std::string getDescription() {
        return manager::LanguageManager::getTranslate("commandEditRankDescription");
    };
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