#pragma once

#include <ll/api/command/EnumName.h>
#include <optional>
#include <string>
#include <unordered_map>

namespace power_ranks::manager {

class CommandManager final {
public:
    enum class RankNames;
    static constexpr inline std::string_view rankEnumNames = ll::command::enum_name_v<RankNames>;

    static bool registerCommands();
    static void addRankNameToSoftEnum(const std::string& rankName);
    static void removeRankNameFromSoftEnum(const std::string& rankName);

    static void setLastWrittedCommand(const std::string& playerName, const std::string& commandName);
    static std::optional<std::string> getAndRemoveLastWrittedCommand(const std::string& playerName);

private:
    static std::unordered_map<std::string, std::string> lastWrittedCommandsByPlayers;
};

} // namespace power_ranks::manager