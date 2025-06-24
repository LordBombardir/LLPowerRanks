#pragma once

#include "HiddenCommandOverloads.h"
#include <optional>

namespace power_ranks::types {

class Rank final {
public:
    Rank(
        const int                              priority,
        const std::string&                     name,
        const std::string&                     prefix,
        const std::string&                     chatFormat,
        const std::string&                     scoreTagFormat,
        const std::optional<const Rank*>&      inheritanceRank        = std::nullopt,
        const std::unordered_set<std::string>& availableCommands      = {},
        const HiddenCommandOverloads&          hiddenCommandOverloads = {},
        const std::vector<std::string>         additionalInformation  = {}
    )
    : priority(priority),
      name(name),
      prefix(prefix),
      chatFormat(chatFormat),
      scoreTagFormat(scoreTagFormat),
      inheritanceRank(inheritanceRank),
      availableCommands(availableCommands),
      hiddenCommandOverloads(hiddenCommandOverloads),
      additionalInformation(additionalInformation) {}
    ~Rank() = default;

    std::string                            getName() const { return name; }
    std::string                            getPrefix() const { return prefix; }
    std::string                            getChatFormat() const { return chatFormat; }
    std::string                            getScoreTagFormat() const { return scoreTagFormat; }
    const std::optional<const Rank*>&      getInheritanceRank() const { return inheritanceRank; }
    const std::unordered_set<std::string>& getAvailableCommands() const { return availableCommands; }
    const HiddenCommandOverloads&          getHiddenCommandOverloads() const { return hiddenCommandOverloads; }
    HiddenCommandOverloads&                getHiddenCommandOverloads() { return hiddenCommandOverloads; }
    const std::vector<std::string>&        getAdditionalInformation() const { return additionalInformation; }

    std::optional<std::string> getAdditionalInformation(size_t index) const;

    void setPrefix(const std::string& prefix);
    void setChatFormat(const std::string& chatFormat);
    void setInheritanceRank(const Rank* inheritanceRank);
    void setScoreTagFormat(const std::string& scoreTagFormat);
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

    std::string                     prefix;
    std::string                     chatFormat;
    std::string                     scoreTagFormat;
    std::optional<const Rank*>      inheritanceRank;
    std::unordered_set<std::string> availableCommands;
    HiddenCommandOverloads          hiddenCommandOverloads;
    std::vector<std::string>        additionalInformation;
};

} // namespace power_ranks::types