#include "AddRankCommand.h"
#include "../Utils.hpp"
#include "../forms/AddRankForm.h"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void AddRankCommand::execute(
    const CommandOrigin& origin,
    CommandOutput&       output,
    const Parameter&     parameter,
    const Command&       _
) {
    std::string localeName = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleName
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleName();

    if (manager::RanksManager::getRank(parameter.rankName).has_value()) {
        output.error(manager::LanguageManager::getInstance()->getTranslate("addRankAlreadyExists", localeName));
        return;
    }

    std::optional<object::Rank*> inheritanceRank = manager::RanksManager::getRank(parameter.inheritanceRank);
    if (parameter.inheritanceRank != "null" && !inheritanceRank.has_value()) {
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

    manager::RanksManager::addRank(
        parameter.rankName,
        parameter.prefix,
        parameter.chatFormat,
        parameter.scoreTagFormat,
        inheritanceRank
    );
    output.success(Utils::strReplace(
        manager::LanguageManager::getInstance()->getTranslate("addRankSuccess", localeName),
        "{rankName}",
        parameter.rankName
    ));
}

void AddRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getInstance()->getTranslate("commandAddRankUsing"));
        return;
    }

    forms::AddRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands