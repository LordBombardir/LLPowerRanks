#include "CommandManager.h"
#include "../../Utils.hpp"
#include "../../commands/AddRankCommand.h"
#include "../../commands/EditRankCommand.h"
#include "../../commands/RemoveRankCommand.h"
#include "../../commands/SetRankCommand.h"
#include "../ranks/RanksManager.h"
#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/service/Bedrock.h>

namespace power_ranks::manager {

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

    for (const std::string& alias : commands::AddRankCommand::getAliases()) {
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

    for (const std::string& alias : commands::SetRankCommand::getAliases()) {
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

    for (const std::string& alias : commands::RemoveRankCommand::getAliases()) {
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

    for (const std::string& alias : commands::EditRankCommand::getAliases()) {
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

bool CommandManager::isCommandAvailable(
    const CommandOrigin&   origin,
    CommandFlag            flags,
    CommandPermissionLevel permissionLevel
) {
    if (origin.getPermissionsLevel() < permissionLevel) {
        return false;
    }

    const CommandOrigin& outputReceiver = origin.getOutputReceiver();
    unsigned int         originType     = static_cast<unsigned int>(outputReceiver.getOriginType());

    bool firstCheck  = false;
    bool secondCheck = false;

    if (originType || (flags.value & CommandFlagValue::HiddenFromPlayerOrigin) == CommandFlagValue::None) {
        if ((originType - 1) <= 1u
            && (flags.value & CommandFlagValue::HiddenFromCommandBlockOrigin) != CommandFlagValue::None) {
            secondCheck = true;
        }
    } else {
        firstCheck = true;
    }

    bool result = (originType - 5) <= 1u
               && (flags.value & CommandFlagValue::HiddenFromAutomationOrigin) != CommandFlagValue::None;
    if (!firstCheck && !secondCheck) {
        return !result;
    }

    return false;
}

} // namespace power_ranks::manager