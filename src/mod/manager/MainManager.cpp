#include "MainManager.h"
#include "../utils/Utils.h"
#include "base/BaseManager.h"
#include "config/ConfigManager.h"
#include "lang/LanguageManager.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include <ll/api/service/Bedrock.h>
#include <mc/server/commands/CommandParameterOption.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/world/actor/ActorDataIDs.h>
#include <mc/world/actor/SynchedActorDataEntityWrapper.h>
#include <mc/world/actor/player/LayeredAbilities.h>
#include <mc/world/level/Level.h>
#include <player_db/Api.h>
#include <translator_api/Api.h>

// wth mojang?
AvailableCommandsPacket::EnumData::EnumData(const EnumData&)                                     = default;
AvailableCommandsPacket::SoftEnumData::SoftEnumData(const SoftEnumData&)                         = default;
AvailableCommandsPacket::ConstrainedValueData::ConstrainedValueData(const ConstrainedValueData&) = default;
AvailableCommandsPacket::ParamData::ParamData(const ParamData&)                                  = default;
AvailableCommandsPacket::OverloadData::OverloadData(const OverloadData&)                         = default;
AvailableCommandsPacket::CommandData::CommandData(const CommandData&)                            = default;

namespace power_ranks::manager {

bool MainManager::initManagers(ll::mod::NativeMod& mod) {
    BaseManager::init(mod);
    bool configInit = ConfigManager::init(mod);
    LanguageManager::init(mod);
    LanguageManager::addTranslations();
    bool rankFormatsInit = RankFormatsManager::init(mod);
    bool ranksInit       = RanksManager::init(mod);

    return configInit && rankFormatsInit && ranksInit;
}

void MainManager::disposeManagers() { RanksManager::dispose(); }

const types::Rank& MainManager::getPlayerRankOrSetDefault(Player& player) {
    const auto& entry = player_db::api::getPlayerEntry(player);

    if (const auto& rankName = BaseManager::getPlayerRank(entry.uuid); rankName.has_value()) {
        if (const auto& rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return **rank;
        }
    }

    const auto& rank = **RanksManager::getRank(ConfigManager::getConfig().defaultRankName);

    setPlayerRank(player, rank);
    return rank;
}

const types::Rank& MainManager::getPlayerRankOrSetDefault(const std::string& playerName) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry(playerName);
    }

    if (const auto& rankName = BaseManager::getPlayerRank(entry->uuid); rankName.has_value()) {
        if (const auto& rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return **rank;
        }
    }

    BaseManager::setPlayerRank(entry->uuid, ConfigManager::getConfig().defaultRankName);
    return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
}

void MainManager::setPlayerRank(Player& player, const types::Rank& rank) {
    const auto& entry = player_db::api::getPlayerEntry(player);

    BaseManager::setPlayerRank(entry.uuid, rank.getName());
    updatePlayerRank(player);
}

void MainManager::setPlayerRankByName(const std::string& playerName, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry(playerName);
    }

    BaseManager::setPlayerRank(entry->uuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayer(playerName); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByXuid(xuid);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry("", xuid);
    }

    BaseManager::setPlayerRank(entry->uuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayerByXuid(xuid); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::updatePlayerRank(Player& player) {
    const types::Rank& rank = getPlayerRankOrSetDefault(player);

    setScoreTag(player, RankFormatsManager::getScoreTagFormat(rank.getName()));

    extraActions(rank, player);
    extraVanillaActions(player, rank);
}

void MainManager::setScoreTag(Player& player, const std::string& scoreTag) {
    // https://github.com/LordBombardir/LLTranslatorApi/issues/2
    // player.mEntityData->set(static_cast<ushort>(ActorDataIDs::Score), scoreTag);
}

void MainManager::extraActions(const types::Rank& rank, const Player& player) {
    optional_ref<CommandRegistry> commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        return;
    }

    AvailableCommandsPacket packet = std::move(commandRegistry->serializeAvailableCommands());
    for (AvailableCommandsPacket::CommandData& command : *packet.mCommands) {
        const auto& commandName = *command.name;
        if (rank.isCommandAvailable(commandName)) {
            command.permission = CommandPermissionLevel::Any;
        }

        if (command.permission == CommandPermissionLevel::Any) {
            if (command.overloads->empty()) {
                continue;
            }

            std::vector<AvailableCommandsPacket::OverloadData> myOverloads = {};
            for (auto [index, overload] : std::views::enumerate(*command.overloads)) {
                if (rank.isCommandOverloadHidden(commandName, static_cast<int>(index))) {
                    continue;
                }

                for (auto& param : *overload.params) {
                    param.paramOptions = static_cast<uchar>(CommandParameterOption::None);
                }

                myOverloads.push_back(std::move(overload));
            }

            command.overloads = std::move(myOverloads);
        }
    }

    for (auto& constraint : *packet.mConstraints) {
        if (constraint.constraints->empty()) {
            continue;
        }

        // выбираем только CommandName и нужные нам команды (packet.mEnumValues->at(constraint.enumValueSymbol)
        // возвращает имя команды)
        if (constraint.enumSymbol != 13
            && !rank.isCommandAvailable(packet.mEnumValues->at(constraint.enumValueSymbol))) {
            continue;
        }

        // убираем constraint
        constraint.constraints = {};
    }

    packet.sendToClient(player.getNetworkIdentifier(), player.getClientSubId());
}

void MainManager::extraVanillaActions(Player& player, const types::Rank& rank) {
    if (rank.isCommandAvailable("teleport")) {
        player.setAbility(AbilitiesIndex::Teleport, true);
    }
}

} // namespace power_ranks::manager