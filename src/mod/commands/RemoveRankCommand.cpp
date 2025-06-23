#include "RemoveRankCommand.h"
#include "../Utils.hpp"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void RemoveRankCommand::execute(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const Parameter&                parameter,
    [[maybe_unused]] const Command& _
) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    const auto& rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks;
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

    manager::RanksManager::removeRank(*rank.value());
    output.success(Utils::strReplace(
        manager::LanguageManager::getTranslate("commandRemoveRankSuccess", localeCode),
        "{rankName}",
        parameter.rankName
    ));
}

void RemoveRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    output.error(manager::LanguageManager::getTranslate("commandRemoveRankUsing", localeCode));
}

} // namespace power_ranks::commands