#include "HiddenCommandOverloads.h"
#include "../Utils.hpp"

namespace power_ranks::types {

std::string HiddenCommandOverloads::toString() const {
    std::unordered_set<std::string> commandEntries;

    for (const auto& [command, overloads] : data) {
        std::unordered_set<std::string> overloadStrs;
        for (int overload : overloads) {
            overloadStrs.insert(std::to_string(overload));
        }

        const auto& joinedOverloads = Utils::separateUnorderedSet(overloadStrs, ",");
        commandEntries.insert(command + ":" + joinedOverloads);
    }

    return Utils::separateUnorderedSet(commandEntries, ";");
}

void HiddenCommandOverloads::updateFromString(const std::string& rawData) { data = getDataFromString(rawData); }

void HiddenCommandOverloads::clearData() { this->data = {}; }

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