#pragma once

#include <string>
#include <unordered_set>

namespace power_ranks::config {

struct Config {
    int                             version                  = 3;
    std::string                     defaultLocaleCode        = "en_US";
    std::string                     defaultRankName          = "Default";
    std::unordered_set<std::string> ranksWithColoredMessages = {"Helper", "Administrator"};
    std::unordered_set<std::string> superRanks               = {"Helper", "Administrator"};
    std::unordered_set<std::string> superPlayers             = {"LordBombardir"};
};

} // namespace power_ranks::config
