#pragma once

#include <optional>
#include <string>
#include <vector>

namespace power_ranks::object {

class Rank {
public:
    Rank();
    Rank(
        const int                         priority,
        const std::string&                name,
        const std::string&                prefix,
        const std::string&                chatFormat,
        const std::string&                scoreTagFormat,
        const std::optional<const Rank*>& inheritanceRank   = std::nullopt,
        const std::vector<std::string>&   availableCommands = {}
    )
    : priority(priority),
      name(name),
      prefix(prefix),
      chatFormat(chatFormat),
      scoreTagFormat(scoreTagFormat),
      inheritanceRank(inheritanceRank),
      availableCommands(availableCommands) {}
    ~Rank() = default;

    std::string                getName() const { return name; }
    std::string                getPrefix() const { return prefix; }
    std::string                getChatFormat() const { return chatFormat; }
    std::string                getScoreTagFormat() const { return scoreTagFormat; }
    std::optional<const Rank*> getInheritanceRank() const { return inheritanceRank; }
    std::vector<std::string>   getAvailableCommands() const { return availableCommands; }

    void setPrefix(const std::string& prefix);
    void setChatFormat(const std::string& chatFormat);
    void setInheritanceRank(const Rank* inheritanceRank);
    void setScoreTagFormat(const std::string& scoreTagFormat);
    void setAvailableCommands(const std::vector<std::string>& availableCommands);

    bool isCommandAvailable(const std::string& name) const;
    void addAvailableCommand(const std::string& name);
    void removeAvailableCommand(const std::string& name);

    void removeInheritanceRank();

    bool operator<(const Rank& other);
    bool operator<=(const Rank& other);
    bool operator>(const Rank& other);
    bool operator>=(const Rank& other);

private:
    const int                  priority;
    const std::string          name;
    std::string                prefix;
    std::string                chatFormat;
    std::string                scoreTagFormat;
    std::optional<const Rank*> inheritanceRank   = std::nullopt;
    std::vector<std::string>   availableCommands = {};
};

} // namespace power_ranks::object