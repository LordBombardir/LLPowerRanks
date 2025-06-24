#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>

namespace power_ranks::types {

struct HiddenCommandOverloads {
public:
    HiddenCommandOverloads() = default;

    explicit HiddenCommandOverloads(const std::unordered_map<std::string, std::unordered_set<int>>& data)
    : data(data) {}

    // Format: teleport:0,2,3,1;gamemode:1
    std::string toString() const;
    void        updateFromString(const std::string& rawData);

    const std::unordered_map<std::string, std::unordered_set<int>>& getData() const { return data; };

private:
    std::unordered_map<std::string, std::unordered_set<int>> data = {};

    static std::unordered_map<std::string, std::unordered_set<int>> getDataFromString(const std::string& rawData);
};

} // namespace power_ranks::types