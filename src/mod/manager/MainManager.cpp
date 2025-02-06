#include "MainManager.h"
#include "../Utils.hpp"
#include "base/BaseManager.h"
#include "config/ConfigManager.h"
#include "lang/LanguageManager.h"
#include "ranks/RanksManager.h"
#include <LLTranslatorApi.h>
#include <ll/api/service/Bedrock.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/world/actor/player/LayeredAbilities.h>
#include <mc/world/level/Level.h>


AvailableCommandsPacket::CommandData::CommandData(const CommandData&) = default;

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

const object::Rank& MainManager::getPlayerRankOrSetDefault(Player& player) {
    std::optional<std::string>   rankName;
    std::optional<std::string>   otherRankName;
    std::optional<object::Rank*> rank;

    if (rankName = BaseManager::getInstance()->getPlayerRankByXuid(player.getXuid()); rankName.has_value()) {
        // clang-format off
        if (otherRankName = BaseManager::getInstance()->getPlayerRankByName(player.getRealName()); !otherRankName.has_value()) {
            // clang-format on
            BaseManager::getInstance()->updatePlayerNameByXuid(player.getXuid(), player.getRealName());
        }

        if (rank = *RanksManager::getRank(rankName.value()); rank.has_value()) {
            return *rank.value();
        }

        BaseManager::getInstance()->updateRankNameByPlayerName(
            player.getRealName(),
            ConfigManager::getConfig().defaultRankName
        );
        return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
    }

    if (rankName = BaseManager::getInstance()->getPlayerRankByName(player.getRealName()); rankName.has_value()) {
        // clang-format off
        if (otherRankName = BaseManager::getInstance()->getPlayerRankByXuid(player.getXuid()); player.getXuid() != "" && !otherRankName.has_value()) {
            // clang-format on
            BaseManager::getInstance()->updateXuidByPlayerName(player.getRealName(), player.getXuid());
        }

        if (rank = *RanksManager::getRank(rankName.value()); rank.has_value()) {
            return *rank.value();
        }

        BaseManager::getInstance()->updateRankNameByPlayerName(
            player.getRealName(),
            ConfigManager::getConfig().defaultRankName
        );
        return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
    }

    rank = RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();

    setPlayerRank(player, *rank.value());
    return *rank.value();
}

const object::Rank& MainManager::getPlayerRankOrSetDefault(const std::string& playerName) {
    // clang-format off
    if (std::optional<std::string> rankName = BaseManager::getInstance()->getPlayerRankByName(playerName); rankName.has_value()) {
        // clang-format on
        if (std::optional<object::Rank*> rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return *rank.value();
        }

        BaseManager::getInstance()->updateRankNameByPlayerName(playerName, ConfigManager::getConfig().defaultRankName);
        return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
    }

    BaseManager::getInstance()->setPlayerRank(playerName, "", ConfigManager::getConfig().defaultRankName);
    return *RanksManager::getRank(ConfigManager::getConfig().defaultRankName).value();
}

void MainManager::setPlayerRank(Player& player, const object::Rank& rank) {
    BaseManager::getInstance()->setPlayerRank(player.getRealName(), player.getXuid(), rank.getName());
    updatePlayerRank(player);
}

void MainManager::setPlayerRankByName(const std::string& playerName, const object::Rank& rank) {
    BaseManager::getInstance()->setPlayerRank(playerName, "", rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayer(playerName); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::setPlayerRankByXuid(const std::string& xuid, const object::Rank& rank) {
    BaseManager::getInstance()->setPlayerRank("", xuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayerByXuid(xuid); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::updatePlayerRank(Player& player) {
    const object::Rank&     rank   = manager::MainManager::getPlayerRankOrSetDefault(player);
    AvailableCommandsPacket packet = getAvailableCommandsPacket(rank, player);

    packet.sendToClient(player.getNetworkIdentifier(), player.getClientSubId());
    player.setScoreTag(Utils::strReplace(rank.getScoreTagFormat(), "{prefix}", rank.getPrefix()));

    extraVanillaActions(player, rank);
}

AvailableCommandsPacket MainManager::getAvailableCommandsPacket(const object::Rank& rank, Player& player) {
    AvailableCommandsPacket packet = ::getAvailableCommandsPacket(player);
    for (AvailableCommandsPacket::CommandData& command : packet.mCommands.get()) {
        std::string commandName = command.name.get();
        if (rank.isCommandAvailable(commandName)) {
            command.permission = CommandPermissionLevel::Any;
        }
    }

    return packet;
}

void MainManager::extraVanillaActions(Player& player, const object::Rank& rank) {
    std::vector<std::string> availableCommands = rank.getAvailableCommands();
    if (std::find(availableCommands.begin(), availableCommands.end(), "teleport") != availableCommands.end()) {
        player.setAbility(AbilitiesIndex::Teleport, true);
    }
}

} // namespace power_ranks::manager