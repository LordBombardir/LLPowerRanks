#include "RemoveRankCommand.h"
#include "../../config/ConfigManager.h"
#include "../../ranks/RanksManager.h"
#include "../../utils/Utils.h"

#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void RemoveRankCommand::execute(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const Parameter&                parameter,
    [[maybe_unused]] const Command& _
) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (parameter.rankName == ConfigManager::getConfig().defaultRankName) {
        output.error(LanguageManager::getTranslate("removeRankDefaultRank", localeCode));
        return;
    }

    if (ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        output.error(LanguageManager::getTranslate("removeRankSuperRank", localeCode));
        return;
    }

    const auto& rank = RanksManager::getRank(parameter.rankName);
    if (!rank.has_value() || rank.value() == nullptr) {
        std::string ranks = "";
        for (const auto& [name, otherRank] : RanksManager::getRanks()) {
            if (ranks.empty()) {
                ranks = name;
                continue;
            }

            ranks += ", " + name;
        }

        output.error(
            Utils::strReplace(
                LanguageManager::getTranslate("undefinedRank", localeCode),
                {"{rankName}", "{ranks}"},
                {parameter.rankName, std::move(ranks)}
            )
        );
        return;
    }

    RanksManager::removeRank(*rank.value());
    output.success(
        Utils::strReplace(
            LanguageManager::getTranslate("removeRankSuccess", localeCode),
            "{rankName}",
            parameter.rankName
        )
    );
}

void RemoveRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    output.error(LanguageManager::getTranslate("commandRemoveRankUsing", localeCode));
}

} // namespace power_ranks::commands
