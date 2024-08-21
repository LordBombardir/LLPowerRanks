#include "Rank.h"
#include <algorithm>

namespace power_ranks::object {

void Rank::setPrefix(const std::string& prefix) { this->prefix = prefix; }
void Rank::setChatFormat(const std::string& chatFormat) { this->chatFormat = chatFormat; }
void Rank::setInheritanceRank(const Rank* inheritanceRank) { this->inheritanceRank = inheritanceRank; }
void Rank::setScoreTagFormat(const std::string& scoreTagFormat) { this->scoreTagFormat = scoreTagFormat; }

bool Rank::isCommandAvailable(const std::string& name) const {
    auto iterator = std::find(availableCommands.begin(), availableCommands.end(), name);
    if (iterator != availableCommands.end()) {
        return true;
    }

    if (inheritanceRank.has_value()) {
        return inheritanceRank.value()->isCommandAvailable(name);
    }

    return false;
}
void Rank::addAvailableCommand(const std::string& name) {
    if (isCommandAvailable(name)) {
        return;
    }

    availableCommands.push_back(name);
}
void Rank::removeAvailableCommand(const std::string& name) {
    auto iterator = std::find(availableCommands.begin(), availableCommands.end(), name);
    if (iterator == availableCommands.end()) {
        return;
    }

    availableCommands.erase(iterator);
}

bool Rank::operator<(const Rank& other) { return priority < other.priority; }
bool Rank::operator<=(const Rank& other) { return priority <= other.priority; }
bool Rank::operator>(const Rank& other) { return priority > other.priority; }
bool Rank::operator>=(const Rank& other) { return priority >= other.priority; }

} // namespace power_ranks::object