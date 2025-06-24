#include "EditRankCommand.h"
#include "../Utils.hpp"
#include "../forms/EditRankForm.h"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void EditRankCommand::executeFirstParameter(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const FirstParameter&           parameter,
    [[maybe_unused]] const Command& _
) {
    // clang-format off
    bool isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    const auto& localeCode = isOriginServer ? manager::ConfigManager::getConfig().defaultLocaleCode : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (!isOriginServer && manager::ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        // clang-format on
        output.error(manager::LanguageManager::getTranslate("editRankSuperRank", localeCode));
        return;
    }

    const auto& rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks = "";
        for (const auto& [name, rank] : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = name;
                continue;
            }

            ranks += ", " + name;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getTranslate("undefinedRank", localeCode),
            {"{rankName}", "{ranks}"},
            {parameter.rankName, std::move(ranks)}
        ));
        return;
    }

    const auto& inheritanceRank = manager::RanksManager::getRank(parameter.inheritanceRank);
    if (parameter.inheritanceRank != "null" && !inheritanceRank.has_value()) {
        std::string ranks = "";
        for (const auto& [name, rank] : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = name;
                continue;
            }

            ranks += ", " + name;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getTranslate("undefinedRank", localeCode),
            {"{rankName}", "{ranks}"},
            {parameter.inheritanceRank, std::move(ranks)}
        ));
        return;
    }

    const auto& availableCommands = Utils::strSplit(parameter.availableCommands, ";");
    if (parameter.availableCommands != "null" && (availableCommands.empty() || availableCommands.front().empty())) {
        output.error(manager::LanguageManager::getTranslate("editRankInvalidFormatAvailableCommands", localeCode));
        return;
    }

    const auto& additionalInformation = Utils::strSplit(parameter.additionalInformation, ";");
    if (parameter.additionalInformation != "null"
        && (additionalInformation.empty() || additionalInformation.front().empty())) {
        output.error(manager::LanguageManager::getTranslate("editRankInvalidFormatAdditionalInformation", localeCode));
        return;
    }

    rank.value()->setPrefix(parameter.prefix);
    rank.value()->setChatFormat(parameter.chatFormat);
    rank.value()->setScoreTagFormat(parameter.scoreTagFormat);

    if (inheritanceRank.has_value()) {
        rank.value()->setInheritanceRank(inheritanceRank.value());
    } else {
        rank.value()->removeInheritanceRank();
    }

    if (parameter.availableCommands != "null") {
        rank.value()->setAvailableCommands({availableCommands.begin(), availableCommands.end()});
    } else {
        rank.value()->setAvailableCommands({});
    }

    rank.value()->getHiddenCommandOverloads().updateFromString(parameter.hiddenCommandOverloads);

    if (parameter.additionalInformation != "null") {
        rank.value()->setAdditionalInformation(additionalInformation);
    } else {
        rank.value()->setAvailableCommands({});
    }

    manager::RanksManager::saveChangesRank(*rank.value());
    output.success(Utils::strReplace(
        manager::LanguageManager::getTranslate("editRankSuccess", localeCode),
        "{rankName}",
        parameter.rankName
    ));
}

void EditRankCommand::executeSecondParameter(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const SecondParameter&          parameter,
    [[maybe_unused]] const Command& _
) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getTranslate("commandEditRankUsing"));
        return;
    }

    ServerPlayer& player = static_cast<ServerPlayer&>(*origin.getEntity());

    if (manager::ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        output.error(manager::LanguageManager::getTranslate("editRankSuperRank", player.getLocaleCode()));
        return;
    }

    const auto& rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks = "";
        for (const auto& [name, rank] : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = name;
                continue;
            }

            ranks += ", " + name;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getTranslate("undefinedRank", player.getLocaleCode()),
            {"{rankName}", "{ranks}"},
            {parameter.rankName, std::move(ranks)}
        ));
        return;
    }

    forms::EditRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()), rank.value());
}

void EditRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    output.error(manager::LanguageManager::getTranslate("commandEditRankUsing", localeCode));
}

} // namespace power_ranks::commands