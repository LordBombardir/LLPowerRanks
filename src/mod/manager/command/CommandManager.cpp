#include "CommandManager.h"
#include "../../Utils.hpp"
#include "../../commands/AddRankCommand.h"
#include "../../commands/RemoveRankCommand.h"
#include "../../commands/SetRankCommand.h"
#include "../../commands/EditRankCommand.h"
#include "../ranks/RanksManager.h"
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/service/Bedrock.h>

namespace power_ranks::manager {

std::unordered_map<std::string, std::string> CommandManager::lastWrittedCommandsByPlayers = {};

bool CommandManager::registerCommands() {
    optional_ref<CommandRegistry> commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        return false;
    }

    std::vector<std::string> rankNames = {};
    for (std::pair<std::string, object::Rank*> pair : RanksManager::getRanks()) {
        rankNames.push_back(pair.first);
    }
    ll::command::CommandRegistrar::getInstance().tryRegisterSoftEnum(std::string{rankEnumNames}, rankNames);

    ll::command::CommandHandle& addRankCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        commands::AddRankCommand::getName(),
        commands::AddRankCommand::getDescription(),
        commands::AddRankCommand::getRequirement(),
        commands::AddRankCommand::getFlag()
    );

    for (std::string alias : commands::AddRankCommand::getAliases()) {
        addRankCommand.alias(alias);
    }

    addRankCommand.overload<commands::AddRankCommand::Parameter>()
        .required("rankName")
        .required("prefix")
        .required("chatFormat")
        .required("scoreTagFormat")
        .optional("inheritanceRank")
        .execute(&commands::AddRankCommand::execute);

    addRankCommand.overload().execute(&commands::AddRankCommand::executeWithoutParameter);

    ll::command::CommandHandle& setRankCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        commands::SetRankCommand::getName(),
        commands::SetRankCommand::getDescription(),
        commands::SetRankCommand::getRequirement(),
        commands::SetRankCommand::getFlag()
    );

    for (std::string alias : commands::SetRankCommand::getAliases()) {
        setRankCommand.alias(alias);
    }

    setRankCommand.overload<commands::SetRankCommand::Parameter>()
        .required("player")
        .required("rankName")
        .execute(&commands::SetRankCommand::execute);

    setRankCommand.overload().execute(&commands::SetRankCommand::executeWithoutParameter);

    ll::command::CommandHandle& removeRankCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        commands::RemoveRankCommand::getName(),
        commands::RemoveRankCommand::getDescription(),
        commands::RemoveRankCommand::getRequirement(),
        commands::RemoveRankCommand::getFlag()
    );

    for (std::string alias : commands::RemoveRankCommand::getAliases()) {
        removeRankCommand.alias(alias);
    }

    removeRankCommand.overload<commands::RemoveRankCommand::Parameter>()
        .required("rankName")
        .execute(&commands::RemoveRankCommand::execute);

    removeRankCommand.overload().execute(&commands::RemoveRankCommand::executeWithoutParameter);

    ll::command::CommandHandle& editRankCommand = ll::command::CommandRegistrar::getInstance().getOrCreateCommand(
        commands::EditRankCommand::getName(),
        commands::EditRankCommand::getDescription(),
        commands::EditRankCommand::getRequirement(),
        commands::EditRankCommand::getFlag()
    );

    for (std::string alias : commands::EditRankCommand::getAliases()) {
        editRankCommand.alias(alias);
    }

    editRankCommand.overload<commands::EditRankCommand::Parameter>()
        .required("rankName")
        .required("prefix")
        .required("chatFormat")
        .required("scoreTagFormat")
        .required("inheritanceRank")
        .required("availableCommands")
        .execute(&commands::EditRankCommand::execute);

    editRankCommand.overload().execute(&commands::EditRankCommand::executeWithoutParameter);
    return true;
}

void CommandManager::addRankNameToSoftEnum(const std::string& rankName) {
    ll::command::CommandRegistrar::getInstance().addSoftEnumValues(std::string{rankEnumNames}, {rankName});
}

void CommandManager::removeRankNameFromSoftEnum(const std::string& rankName) {
    ll::command::CommandRegistrar::getInstance().removeSoftEnumValues(std::string{rankEnumNames}, {rankName});
}

void CommandManager::setLastWrittedCommand(const std::string& playerName, const std::string& commandName) {
    lastWrittedCommandsByPlayers[Utils::strToLower(playerName)] = commandName;
}

std::optional<std::string> CommandManager::getAndRemoveLastWrittedCommand(const std::string& playerName) {
    if (!lastWrittedCommandsByPlayers.contains(Utils::strToLower(playerName))) {
        return std::nullopt;
    }

    std::optional<std::string> result = lastWrittedCommandsByPlayers[Utils::strToLower(playerName)];

    lastWrittedCommandsByPlayers.erase(Utils::strToLower(playerName));
    return result;
}
} // namespace power_ranks::manager