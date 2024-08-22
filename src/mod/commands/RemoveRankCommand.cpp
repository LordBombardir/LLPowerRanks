#include "RemoveRankCommand.h"
#include "../Utils.hpp"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void RemoveRankCommand::execute(
    const CommandOrigin& origin,
    CommandOutput&       output,
    const Parameter&     parameter,
    const Command&       _
) {
    std::string localeName = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleName
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleName();

    std::optional<object::Rank*> rank = manager::RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks = "";
        for (std::pair<std::string, object::Rank*> pair : manager::RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = pair.first;
                continue;
            }

            ranks += ", " + pair.first;
        }

        output.error(Utils::strReplace(
            manager::LanguageManager::getInstance()->getTranslate("undefinedRank", localeName),
            {"{rankName}", "{ranks}"},
            {parameter.rankName, ranks}
        ));
        return;
    }

    manager::RanksManager::removeRank(*rank.value());
    output.success(Utils::strReplace(
        manager::LanguageManager::getInstance()->getTranslate("commandRemoveRankSuccess", localeName),
        "{rankName}",
        parameter.rankName
    ));
}

void RemoveRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    std::string localeName = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleName
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleName();

    output.error(manager::LanguageManager::getInstance()->getTranslate("commandRemoveRankUsing", localeName));
}

} // namespace power_ranks::commands