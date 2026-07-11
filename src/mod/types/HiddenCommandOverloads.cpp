#include "HiddenCommandOverloads.h"
#include "utils/Utils.h"

namespace power_ranks::types {

std::string HiddenCommandOverloads::toString() const {
    std::vector<std::string> commandEntries;
    commandEntries.reserve(data.size());

    for (const auto& [command, overloads] : data) {
        std::vector<std::string> overloadStrs;
        overloadStrs.reserve(overloads.size());
        for (int overload : overloads) {
            overloadStrs.push_back(std::to_string(overload));
        }

        const auto& joinedOverloads = Utils::separateContainer(overloadStrs, ",");
        commandEntries.push_back(command + ":" + joinedOverloads);
    }

    return Utils::separateContainer(commandEntries, ";");
}

void HiddenCommandOverloads::updateFromString(const std::string& rawData) { data = getDataFromString(rawData); }

std::unordered_map<std::string, std::unordered_set<int>>
HiddenCommandOverloads::getDataFromString(const std::string& rawData) {
    std::unordered_map<std::string, std::unordered_set<int>> result;

    const auto& commandParts = Utils::strSplit(rawData, ";");
    for (const auto& entry : commandParts) {
        const auto& parts = Utils::strSplit(entry, ":");
        if (parts.size() != 2) {
            continue;
        }

        const auto& command         = parts[0];
        const auto& overloadStrings = Utils::strSplit(parts[1], ",");

        std::unordered_set<int> overloads;
        for (const auto& str : overloadStrings) {
            if (!str.empty()) {
                overloads.insert(std::stoi(str));
            }
        }

        if (overloads.empty()) {
            continue;
        }

        result[command] = std::move(overloads);
    }

    return result;
}

} // namespace power_ranks::types
