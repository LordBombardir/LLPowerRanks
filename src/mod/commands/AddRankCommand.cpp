#include "AddRankCommand.h"
#include "../forms/AddRankForm.h"
#include "../manager/ranks/RanksManager.h"
#include "../utils/Utils.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void AddRankCommand::execute(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const Parameter&                parameter,
    [[maybe_unused]] const Command& _
) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? manager::ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (manager::RanksManager::getRank(parameter.rankName).has_value()) {
        output.error(manager::LanguageManager::getTranslate("addRankAlreadyExists", localeCode));
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

        output.error(
            Utils::strReplace(
                manager::LanguageManager::getTranslate("undefinedRank", localeCode),
                {"{rankName}", "{ranks}"},
                {parameter.rankName, std::move(ranks)}
            )
        );
        return;
    }

    manager::RanksManager::addRank(
        parameter.rankName,
        parameter.prefix,
        parameter.chat,
        parameter.scoreTag,
        inheritanceRank
    );
    output.success(
        Utils::strReplace(
            manager::LanguageManager::getTranslate("addRankSuccess", localeCode),
            "{rankName}",
            parameter.rankName
        )
    );
}

void AddRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getTranslate("commandAddRankUsing"));
        return;
    }

    forms::AddRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands