#include "EditRankCommand.h"
#include "../forms/EditRankForm.h"
#include "../Utils.hpp"
#include "../manager/ranks/RanksManager.h"
#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void EditRankCommand::execute(
    const CommandOrigin& origin,
    CommandOutput&       output,
    const Parameter&     parameter,
    const Command&       _
) {
    // clang-format off
    bool isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    std::string localeName = isOriginServer ? manager::ConfigManager::getConfig().defaultLocaleName : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleName();

    if (!isOriginServer && Utils::isValueInVector(manager::ConfigManager::getConfig().superRanks, std::string{parameter.rankName})) {
        // clang-format on
        output.error(manager::LanguageManager::getInstance()->getTranslate("editRankSuperRank", localeName));
        return;
    }

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

    std::vector<std::string> availableCommands = Utils::strSplit(parameter.availableCommands, ";");
    if (parameter.availableCommands != "null" && availableCommands.empty()) {
        output.error(
            manager::LanguageManager::getInstance()->getTranslate("editRankInvalidFormatAvailableCommands", localeName)
        );
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
        manager::LanguageManager::getInstance()->getTranslate("editRankSuccess", localeName),
        "{rankName}",
        parameter.rankName
    ));
}

void EditRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(manager::LanguageManager::getInstance()->getTranslate("commandEditRankUsing"));
        return;
    }

    forms::EditRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()));
}

} // namespace power_ranks::commands