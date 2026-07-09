#include "CommandManager.h"
#include "../ranks/RanksManager.h"
#include "../utils/Utils.h"
#include "types/AddRankCommand.h"
#include "types/EditRankCommand.h"
#include "types/RemoveRankCommand.h"
#include "types/SetRankCommand.h"

#include <ll/api/command/CommandHandle.h>
#include <ll/api/command/CommandRegistrar.h>
#include <ll/api/service/Bedrock.h>

namespace power_ranks {

bool CommandManager::registerCommands() {
    std::vector<std::string> rankNames = {};
    for (const auto& [name, rank] : RanksManager::getRanks()) {
        rankNames.push_back(name);
    }

    ll::command::CommandRegistrar::getInstance(false).tryRegisterSoftEnum(std::string{rankEnumNames}, rankNames);

    ll::command::CommandHandle& addRankCommand = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
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
        .required("chat")
        .required("scoreTag")
        .optional("inheritanceRank")
        .execute(&commands::AddRankCommand::execute);

    addRankCommand.overload().execute(&commands::AddRankCommand::executeWithoutParameter);

    ll::command::CommandHandle& setRankCommand = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
        commands::SetRankCommand::getName(),
        commands::SetRankCommand::getDescription(),
        commands::SetRankCommand::getRequirement(),
        commands::SetRankCommand::getFlag()
    );

    for (const std::string& alias : commands::SetRankCommand::getAliases()) {
        setRankCommand.alias(alias);
    }

    setRankCommand.overload<commands::SetRankCommand::Parameter>()
        .required("rankName")
        .optional("player")
        .execute(&commands::SetRankCommand::execute);

    setRankCommand.overload().execute(&commands::SetRankCommand::executeWithoutParameter);

    ll::command::CommandHandle& removeRankCommand =
        ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
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

    ll::command::CommandHandle& editRankCommand = ll::command::CommandRegistrar::getInstance(false).getOrCreateCommand(
        commands::EditRankCommand::getName(),
        commands::EditRankCommand::getDescription(),
        commands::EditRankCommand::getRequirement(),
        commands::EditRankCommand::getFlag()
    );

    for (const std::string& alias : commands::EditRankCommand::getAliases()) {
        editRankCommand.alias(alias);
    }

    editRankCommand.overload<commands::EditRankCommand::FirstParameter>()
        .required("rankName")
        .required("localeCode")
        .execute(&commands::EditRankCommand::executeFirstParameter);

    editRankCommand.overload<commands::EditRankCommand::SecondParameter>()
        .required("rankName")
        .required("prefix")
        .required("chat")
        .required("scoreTag")
        .required("inheritanceRank")
        .required("availableCommands")
        .required("hiddenCommandOverloads")
        .required("additionalInformation")
        .execute(&commands::EditRankCommand::executeSecondParameter);

    editRankCommand.overload().execute(&commands::EditRankCommand::executeWithoutParameter);
    return true;
}

void CommandManager::addRankNameToSoftEnum(const std::string& rankName) {
    ll::command::CommandRegistrar::getInstance(false).addSoftEnumValues(std::string{rankEnumNames}, {rankName});
}

void CommandManager::removeRankNameFromSoftEnum(const std::string& rankName) {
    ll::command::CommandRegistrar::getInstance(false).removeSoftEnumValues(std::string{rankEnumNames}, {rankName});
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

} // namespace power_ranks
