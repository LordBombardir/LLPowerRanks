#include "MainManager.h"
#include "commands/CommandManager.h"
#include "config/ConfigManager.h"
#include "config/types/Config.h"
#include "database/BaseManager.h"
#include "hooks/Hooks.h"
#include "lang/LanguageManager.h"
#include "rankFormats/RankFormatsManager.h"
#include "ranks/RanksManager.h"
#include "utils/Utils.h"

#include <ll/api/event/EventBus.h>
#include <ll/api/event/command/ServerCommandRegisterEvent.h>
#include <ll/api/event/player/PlayerJoinEvent.h>
#include <ll/api/service/Bedrock.h>
#include <mc/server/commands/CommandParameterOption.h>
#include <mc/server/commands/CommandRegistry.h>
#include <mc/world/actor/ActorDataIDs.h>
#include <mc/world/actor/SynchedActorDataEntityWrapper.h>
#include <mc/world/actor/player/LayeredAbilities.h>
#include <mc/world/level/Level.h>
#include <placeholder_api/Api.h>
#include <player_db/Api.h>

// wth mojang?
AvailableCommandsPacket::CommandData::CommandData(const CommandData&) = default;

namespace power_ranks {

ll::event::ListenerPtr MainManager::mCommandRegisterListener = nullptr;
ll::event::ListenerPtr MainManager::mPlayerJoinListener      = nullptr;

bool MainManager::initModWhileLoading(ll::mod::NativeMod& mod) {
    BaseManager::init(mod);

    if (!ConfigManager::init(mod)) {
        mod.getLogger().error("Failed to init ConfigManager!");
        return false;
    }

    if (!LanguageManager::init(mod)) {
        mod.getLogger().error("Failed to init LanguageManager!");
        return false;
    }

    if (!RankFormatsManager::init(mod)) {
        mod.getLogger().error("Failed to init RankFormatsManager!");
        return false;
    }

    if (!RanksManager::init(mod)) {
        mod.getLogger().error("Failed to init RanksManager!");
        return false;
    }

    Hooks::setup();
    return true;
}

bool MainManager::initModWhileEnabling([[maybe_unused]] ll::mod::NativeMod& mod) {
    auto& bus = ll::event::EventBus::getInstance();

    mCommandRegisterListener =
        bus.emplaceListener<ll::event::ServerCommandRegisterEvent>([](ll::event::ServerCommandRegisterEvent&) {
            CommandManager::registerCommands();
        });
    mPlayerJoinListener = bus.emplaceListener<ll::event::PlayerJoinEvent>([](ll::event::PlayerJoinEvent& event) {
        updatePlayerRank(event.self());
    });

    return true;
}

void MainManager::disableMod() {
    RanksManager::dispose();

    auto& bus = ll::event::EventBus::getInstance();

    if (mCommandRegisterListener) {
        bus.removeListener(mCommandRegisterListener);
        mCommandRegisterListener.reset();
    }

    if (mPlayerJoinListener) {
        bus.removeListener(mPlayerJoinListener);
        mPlayerJoinListener.reset();
    }
}

const types::Rank& MainManager::getPlayerRankOrSetDefault(Player& player) {
    const auto entry = player_db::api::getPlayerEntry(player);

    if (const auto rankName = BaseManager::getPlayerRank(entry.uuid); rankName.has_value()) {
        if (const auto rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return **rank;
        }
    }

    const auto rank = *RanksManager::getRank(ConfigManager::getConfig().defaultRankName);

    setPlayerRank(player, *rank);
    return *rank;
}

const types::Rank& MainManager::getPlayerRankOrSetDefault(const std::string& playerName) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addUnknownPlayerEntry(playerName);
    }

    if (const auto rankName = BaseManager::getPlayerRank(entry->uuid); rankName.has_value()) {
        if (const auto rank = RanksManager::getRank(rankName.value()); rank.has_value()) {
            return **rank;
        }
    }

    BaseManager::setPlayerRank(entry->uuid, ConfigManager::getConfig().defaultRankName);
    return **RanksManager::getRank(ConfigManager::getConfig().defaultRankName);
}

void MainManager::setPlayerRank(Player& player, const types::Rank& rank) {
    const auto entry = player_db::api::getPlayerEntry(player);

    BaseManager::setPlayerRank(entry.uuid, rank.getName());
    updatePlayerRank(player);
}

void MainManager::setPlayerRankByName(const std::string& playerName, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByName(playerName);
    if (!entry.has_value()) {
        entry = player_db::api::addUnknownPlayerEntry(playerName);
    }

    BaseManager::setPlayerRank(entry->uuid, rank.getName());
    if (Player* player = ll::service::getLevel()->getPlayer(playerName); player != nullptr) {
        updatePlayerRank(*player);
    }
}

void MainManager::setPlayerRankByXuid(const std::string& xuid, const types::Rank& rank) {
    auto entry = player_db::api::getPlayerEntryByXuid(xuid);
    if (!entry.has_value()) {
        entry = player_db::api::addUnknownPlayerEntry("", xuid);
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
    player.mEntityData->set(static_cast<ushort>(ActorDataIDs::Score), scoreTag);
}

void MainManager::extraActions(const types::Rank& rank, const Player& player) {
    optional_ref<CommandRegistry> commandRegistry = ll::service::getCommandRegistry();
    if (!commandRegistry) {
        return;
    }

    AvailableCommandsPacket packet = commandRegistry->serializeAvailableCommands();
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
            size_t                                             index       = 0;
            for (auto& overload : *command.overloads) {
                if (rank.isCommandOverloadHidden(commandName, static_cast<int>(index))) {
                    index++;
                    continue;
                }

                for (auto& param : *overload.params) {
                    param.paramOptions = static_cast<uchar>(CommandParameterOption::None);
                }

                myOverloads.push_back(std::move(overload));
                index++;
            }

            command.overloads = std::move(myOverloads);
        }
    }

    for (auto& constraint : *packet.mConstraints) {
        if (constraint.constraints->empty()) {
            continue;
        }

        if (constraint.enumSymbol != 13
            && !rank.isCommandAvailable(packet.mEnumValues->at(constraint.enumValueSymbol))) {
            continue;
        }

        constraint.constraints = {};
    }

    packet.sendToClient(player.getNetworkIdentifier(), player.getClientSubId());
}

void MainManager::extraVanillaActions(Player& player, const types::Rank& rank) {
    if (rank.isCommandAvailable("teleport")) {
        player.setAbility(AbilitiesIndex::Teleport, true);
    }
}

} // namespace power_ranks
