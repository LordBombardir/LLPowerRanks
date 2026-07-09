#pragma once

#include <ll/api/command/EnumName.h>
#include <mc/server/commands/CommandFlag.h>
#include <mc/server/commands/CommandOrigin.h>
#include <string>

namespace power_ranks {

class CommandManager final {
public:
    enum class RankNames;
    static constexpr inline std::string_view rankEnumNames = ll::command::enum_name_v<RankNames>;

    static bool registerCommands();
    static void addRankNameToSoftEnum(const std::string& rankName);
    static void removeRankNameFromSoftEnum(const std::string& rankName);

    static bool
    isCommandAvailable(const CommandOrigin& origin, CommandFlag flags, CommandPermissionLevel permissionLevel);
};

} // namespace power_ranks
