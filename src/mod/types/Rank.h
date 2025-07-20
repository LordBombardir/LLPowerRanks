#pragma once

#include "HiddenCommandOverloads.h"
#include <optional>

namespace power_ranks::types {

class Rank final {
public:
    Rank(
        const int                              priority,
        const std::string&                     name,
        const std::optional<const Rank*>&      inheritanceRank        = std::nullopt,
        const std::unordered_set<std::string>& availableCommands      = {},
        const HiddenCommandOverloads&          hiddenCommandOverloads = {},
        const std::vector<std::string>         additionalInformation  = {}
    )
    : priority(priority),
      name(name),
      inheritanceRank(inheritanceRank),
      availableCommands(availableCommands),
      hiddenCommandOverloads(hiddenCommandOverloads),
      additionalInformation(additionalInformation) {}
    ~Rank() = default;

    std::string                            getName() const { return name; }
    const std::optional<const Rank*>&      getInheritanceRank() const { return inheritanceRank; }
    const std::unordered_set<std::string>& getAvailableCommands() const { return availableCommands; }
    const HiddenCommandOverloads&          getHiddenCommandOverloads() const { return hiddenCommandOverloads; }
    HiddenCommandOverloads&                getHiddenCommandOverloads() { return hiddenCommandOverloads; }
    const std::vector<std::string>&        getAdditionalInformation() const { return additionalInformation; }

    std::optional<std::string> getAdditionalInformation(size_t index) const;

    void setInheritanceRank(const Rank* inheritanceRank);
    void setAvailableCommands(const std::unordered_set<std::string>& availableCommands);
    void setAdditionalInformation(const std::vector<std::string>& additionalInformation);

    bool isCommandAvailable(const std::string& name) const;
    bool isCommandOverloadHidden(const std::string& name, int overloadIndex) const;

    void removeInheritanceRank();

    bool operator<(const Rank& other) const;
    bool operator<=(const Rank& other) const;
    bool operator>(const Rank& other) const;
    bool operator>=(const Rank& other) const;

private:
    Rank() = delete;

    const int         priority;
    const std::string name;

    std::optional<const Rank*>      inheritanceRank;
    std::unordered_set<std::string> availableCommands;
    HiddenCommandOverloads          hiddenCommandOverloads;
    std::vector<std::string>        additionalInformation;
};

} // namespace power_ranks::types