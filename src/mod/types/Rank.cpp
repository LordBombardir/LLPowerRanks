#include "Rank.h"

namespace power_ranks::types {

std::optional<std::string> Rank::getAdditionalInformation(size_t index) const {
    if (index < additionalInformation.size()) {
        return additionalInformation[index];
    }

    return std::nullopt;
}

void Rank::setInheritanceRank(const Rank* inheritanceRank) { this->inheritanceRank = inheritanceRank; }

void Rank::setAvailableCommands(const std::unordered_set<std::string>& availableCommands) {
    this->availableCommands = availableCommands;
}

void Rank::setAdditionalInformation(const std::vector<std::string>& additionalInformation) {
    this->additionalInformation = additionalInformation;
}

bool Rank::isCommandAvailable(const std::string& name) const {
    if (availableCommands.contains(name)) {
        return true;
    }

    if (inheritanceRank.has_value()) {
        return inheritanceRank.value()->isCommandAvailable(name);
    }

    return false;
}

bool Rank::isCommandOverloadHidden(const std::string& name, int overloadIndex) const {
    auto it = hiddenCommandOverloads.getData().find(name);
    if (it == hiddenCommandOverloads.getData().end()) {
        return false;
    }

    return it->second.contains(overloadIndex);
}

void Rank::removeInheritanceRank() { inheritanceRank = std::nullopt; }

bool Rank::operator<(const Rank& other) const { return priority < other.priority; }

bool Rank::operator<=(const Rank& other) const { return priority <= other.priority; }

bool Rank::operator>(const Rank& other) const { return priority > other.priority; }

bool Rank::operator>=(const Rank& other) const { return priority >= other.priority; }

} // namespace power_ranks::types