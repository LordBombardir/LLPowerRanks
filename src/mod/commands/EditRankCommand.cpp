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
    std::string localeCode = isOriginServer ? manager::ConfigManager::getConfig().defaultLocaleCode : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (!isOriginServer && manager::ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        // clang-format on
        output.error(manager::LanguageManager::getTranslate("editRankSuperRank", localeCode));
        return;
    }

    std::optional<types::Rank*> rank = manager::RanksManager::getRank(parameter.rankName);
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
            {parameter.rankName, ranks}
        ));
        return;
    }

    std::optional<types::Rank*> inheritanceRank = manager::RanksManager::getRank(parameter.inheritanceRank);
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
            {parameter.inheritanceRank, ranks}
        ));
        return;
    }

    std::vector<std::string> availableCommands = Utils::strSplit(parameter.availableCommands, ";");
    if (parameter.availableCommands != "null" && availableCommands.empty()) {
        output.error(manager::LanguageManager::getTranslate("editRankInvalidFormatAvailableCommands", localeCode));
        return;
    }

    rank.value()->setPrefix(parameter.prefix);
    rank.value()->setChatFormat(parameter.chatFormat);
    rank.value()->setScoreTagFormat(parameter.scoreTagFormat);
    rank.value()->setAvailableCommands(availableCommands);

    if (inheritanceRank.has_value()) {
        rank.value()->setInheritanceRank(inheritanceRank.value());
    } else {
        rank.value()->removeInheritanceRank();
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

    std::optional<types::Rank*> rank = manager::RanksManager::getRank(parameter.rankName);
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
            {parameter.rankName, ranks}
        ));
        return;
    }

    forms::EditRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()), rank.value());
}

void EditRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    std::string localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    output.error(manager::LanguageManager::getTranslate("commandEditRankUsing", localeCode));
}

} // namespace power_ranks::commands