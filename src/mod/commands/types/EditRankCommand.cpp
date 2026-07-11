#include "EditRankCommand.h"
#include "config/ConfigManager.h"
#include "forms/EditRankForm.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include "utils/Utils.h"

#include <mc/server/ServerPlayer.h>

namespace power_ranks::commands {

void EditRankCommand::executeFirstParameter(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const FirstParameter&           parameter,
    [[maybe_unused]] const Command& _
) {
    if (origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)) {
        output.error(LanguageManager::getTranslate("commandEditRankUsing"));
        return;
    }

    ServerPlayer& player = static_cast<ServerPlayer&>(*origin.getEntity());

    if (ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        output.error(LanguageManager::getTranslate("editRankSuperRank", player.getLocaleCode()));
        return;
    }

    if (!RankFormatsManager::isKnownLocaleCode(parameter.localeCode, true)) {
        output.error(
            Utils::strReplace(
                LanguageManager::getTranslate("editRankSecondInvalidLocaleCode", player.getLocaleCode()),
                "{defaultLocaleCode}",
                ConfigManager::getConfig().defaultLocaleCode
            )
        );
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
                LanguageManager::getTranslate("undefinedRank", player.getLocaleCode()),
                {"{rankName}", "{ranks}"},
                {parameter.rankName, std::move(ranks)}
            )
        );
        return;
    }

    forms::EditRankForm::init(static_cast<ServerPlayer&>(*origin.getEntity()), rank.value(), parameter.localeCode);
}

void EditRankCommand::executeSecondParameter(
    const CommandOrigin&            origin,
    CommandOutput&                  output,
    const SecondParameter&          parameter,
    [[maybe_unused]] const Command& _
) {
    bool        isOriginServer = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player);
    const auto& localeCode     = isOriginServer ? ConfigManager::getConfig().defaultLocaleCode
                                                : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    if (!isOriginServer && ConfigManager::getConfig().superRanks.contains(parameter.rankName)) {
        output.error(LanguageManager::getTranslate("editRankSuperRank", localeCode));
        return;
    }

    if (!RankFormatsManager::isKnownLocaleCode(parameter.localeCode, true)) {
        output.error(LanguageManager::getTranslate("editRankFirstInvalidLocaleCode", localeCode));
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

    const auto& inheritanceRank = RanksManager::getRank(parameter.inheritanceRank);
    if (parameter.inheritanceRank != "null" && !inheritanceRank.has_value()) {
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
                {parameter.inheritanceRank, std::move(ranks)}
            )
        );
        return;
    }

    const auto& availableCommands = Utils::strSplit(parameter.availableCommands, ";");
    if (parameter.availableCommands != "null" && (availableCommands.empty() || availableCommands.front().empty())) {
        output.error(LanguageManager::getTranslate("editRankInvalidFormatAvailableCommands", localeCode));
        return;
    }

    const auto& additionalInformation = Utils::strSplit(parameter.additionalInformation, ";");
    if (parameter.additionalInformation != "null"
        && (additionalInformation.empty() || additionalInformation.front().empty())) {
        output.error(LanguageManager::getTranslate("editRankInvalidFormatAdditionalInformation", localeCode));
        return;
    }

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
        rank.value()->setAdditionalInformation({});
    }

    RankFormatsManager::setRankFormat(
        rank.value()->getName(),
        parameter.prefix,
        parameter.chat,
        parameter.scoreTag,
        parameter.localeCode
    );
    RanksManager::saveChangesRank(*rank.value());

    output.success(
        Utils::strReplace(
            LanguageManager::getTranslate("editRankSuccess", localeCode),
            "{rankName}",
            parameter.rankName
        )
    );
}

void EditRankCommand::executeWithoutParameter(const CommandOrigin& origin, CommandOutput& output) {
    const auto& localeCode = origin.getEntity() == nullptr || !origin.getEntity()->isType(ActorType::Player)
                               ? ConfigManager::getConfig().defaultLocaleCode
                               : static_cast<ServerPlayer&>(*origin.getEntity()).getLocaleCode();

    output.error(LanguageManager::getTranslate("commandEditRankUsing", localeCode));
}

} // namespace power_ranks::commands
