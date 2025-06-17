#include "MainManager.h"
#include "../Utils.hpp"
#include "base/BaseManager.h"
#include "config/ConfigManager.h"
#include "lang/LanguageManager.h"
#include "ranks/RanksManager.h"
#include <ll/api/service/Bedrock.h>
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
    try {
        BaseManager::init(mod);
        bool configInit = ConfigManager::init(mod);
        LanguageManager::init(mod);
        LanguageManager::addTranslations();
        bool ranksInit = RanksManager::init(mod);

        return configInit && ranksInit;
    } catch (...) {
        return false;
    }
}

void MainManager::disposeManagers() {
    BaseManager::dispose();
    RanksManager::dispose();
}

const types::Rank& MainManager::getPlayerRankOrSetDefault(Player& player) {
    std::optional<types::Rank*> rank;

    const auto& entry = player_db::api::getPlayerEntry(player);

    if (std::optional<std::string> rankName = BaseManager::getInstance()->getPlayerRank(entry.uuid);
        rankName.has_value()) {
        if (rank = *RanksManager::getRank(rankName.value()); rank.has_value()) {
            return *rank.value();
        }

        BaseManager::getInstance()->setPlayerRank(entry.uuid, ConfigManager::getConfig().defaultRankName);
        return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
    }

    rank = RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();

    setPlayerRank(player, *rank.value());
    return *rank.value();
}

const types::Rank& MainManager::getPlayerRankOrSetDefault(const std::string& playerName) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry(playerName);
    }

    if (std::optional<std::string> rankName = BaseManager::getInstance()->getPlayerRank(entry->uuid);
        rankName.has_value()) {
        if (std::optional<types::Rank*> rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return *rank.value();
        }

        BaseManager::getInstance()->setPlayerRank(entry->uuid, ConfigManager::getConfig().defaultRankName);
        return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
    }

    BaseManager::getInstance()->setPlayerRank(entry->uuid, ConfigManager::getConfig().defaultRankName);
    return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
}

void MainManager::setPlayerRank(Player& player, const types::Rank& rank) {
    const auto& entry = player_db::api::getPlayerEntry(player);

    BaseManager::getInstance()->setPlayerRank(entry.uuid, rank.getName());
    updatePlayerRank(player);
}

void MainManager::setPlayerRankByName(const std::string& playerName, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry(playerName);
    }

    BaseManager::getInstance()->setPlayerRank(entry->uuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayer(playerName); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByXuid(xuid);
    if (!entry.has_value()) {
        entry = player_db::api::addTemporaryPlayerEntry("", xuid);
    }

    BaseManager::getInstance()->setPlayerRank(entry->uuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayerByXuid(xuid); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::updatePlayerRank(Player& player) {
    const types::Rank& rank = manager::MainManager::getPlayerRankOrSetDefault(player);

    setScoreTag(player, Utils::strReplace(rank.getScoreTagFormat(), "{prefix}", rank.getPrefix()));

    extraActions(rank, player);
    extraVanillaActions(player, rank);
}

void MainManager::setScoreTag(Player& player, const std::string& scoreTag) {
    player.mEntityData->set(static_cast<ushort>(ActorDataIDs::Score), scoreTag);
}

void MainManager::extraActions(const types::Rank& rank, const Player& player) {
    AvailableCommandsPacket packet = translator::api::getAvailableCommandsPacket(player);
    for (AvailableCommandsPacket::CommandData& command : packet.mCommands.get()) {
        std::string commandName = command.name.get();
        if (rank.isCommandAvailable(commandName)) {
            command.permission = CommandPermissionLevel::Any;
        }
    }

    packet.sendToClient(player.getNetworkIdentifier(), player.getClientSubId());
}

void MainManager::extraVanillaActions(Player& player, const types::Rank& rank) {
    std::vector<std::string> availableCommands = rank.getAvailableCommands();
    if (std::find(availableCommands.begin(), availableCommands.end(), "teleport") != availableCommands.end()) {
        player.setAbility(AbilitiesIndex::Teleport, true);
    }
}

} // namespace power_ranks::manager