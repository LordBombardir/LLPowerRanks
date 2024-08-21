#pragma once

#include "../manager/lang/LanguageManager.h"
#include <mc/world/actor/player/Player.h>
#include <fmt/std.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <mc/server/commands/CommandOutput.h>
#include <mc/server/commands/CommandPermissionLevel.h>
#include <mc/server/commands/CommandSelector.h>

namespace power_ranks::commands {

class SetRankCommand {
public:
    struct Parameter {
        CommandSelector<Player> player;
        std::string                   rankName{manager::ConfigManager::getConfig().defaultRankName};
    };

    static std::string getName() { return "setrank"; };
    static std::string getDescription() {
        return manager::LanguageManager::getInstance()->getTranslate("commandSetRankDescription");
    };
    static CommandPermissionLevel getRequirement() { return CommandPermissionLevel::GameDirectors; };
    static CommandFlag            getFlag() { return CommandFlagValue::NotCheat; };

    static std::vector<std::string> getAliases() { return {"set-rank"}; };

    static void
    execute(const CommandOrigin& origin, CommandOutput& output, const Parameter& parameter, const Command& command);
    static void executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output);
};

} // namespace power_ranks::commands