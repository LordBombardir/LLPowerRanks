#include "BaseManager.h"
#include "../../Utils.hpp"

namespace power_ranks::manager {

std::unique_ptr<base::pool::ConnectionPool> BaseManager::connectionPool = nullptr;
BaseManager*                                BaseManager::instance       = nullptr;

BaseManager::BaseManager(ll::mod::NativeMod& mod) {
    connectionPool = std::make_unique<base::pool::ConnectionPool>(
        base::pool::ConnectionPool(Utils::fixPath(mod.getDataDir().string() + "/players.db"))
    );
    connectionPool->setupDataBase([](sqlite3* db) -> void {
        char* errorMessage;

        std::string createPlayersTableQuery = "CREATE TABLE IF NOT EXISTS `players` "
                                              "("
                                              "`Name` TEXT DEFAULT 'null', "
                                              "`XUID` TEXT DEFAULT 'null', "
                                              "`RankName` TEXT NOT NULL"
                                              ");";

        sqlite3_exec(db, createPlayersTableQuery.c_str(), nullptr, nullptr, &errorMessage);
        sqlite3_free(errorMessage);
    });
}

void BaseManager::init(ll::mod::NativeMod& mod) { instance = new BaseManager(mod); }
void BaseManager::dispose() { delete getInstance(); }

std::optional<std::string> BaseManager::getPlayerRankByName(const std::string& playerName) {
    return connectionPool->executeSelectQuery(
        "SELECT `RankName` FROM `players` WHERE `Name` = ?;",
        {Utils::strToLower(playerName)}
    );
}
std::optional<std::string> BaseManager::getPlayerRankByXuid(const std::string& xuid) {
    return connectionPool->executeSelectQuery("SELECT `RankName` FROM `players` WHERE `XUID` = ?;", {xuid});
}

bool BaseManager::setPlayerRank(const std::string& playerName, const std::string& xuid, const std::string& rankName) {
    if (getPlayerRankByName(playerName).has_value() || getPlayerRankByXuid(xuid).has_value()) {
        return updateRankNameByPlayerName(playerName, rankName);
    }

    if (xuid == "") {
        return connectionPool->executeUpdateQuery(
            "INSERT INTO `players` (`Name`, `RankName`) VALUES (?, ?);",
            {Utils::strToLower(playerName), rankName}
        );
    }

    return connectionPool->executeUpdateQuery(
        "INSERT INTO `players` (`Name`, `XUID`, `RankName`) VALUES (?, ?, ?);",
        {Utils::strToLower(playerName), xuid, rankName}
    );
}

bool BaseManager::updatePlayerNameByXuid(const std::string& xuid, const std::string& playerName) {
    return connectionPool->executeUpdateQuery(
        "UPDATE `players` SET `Name` = ? WHERE `XUID` = ?;",
        {Utils::strToLower(playerName), xuid}
    );
}

bool BaseManager::updateXuidByPlayerName(const std::string& playerName, const std::string& xuid) {
    return connectionPool->executeUpdateQuery(
        "UPDATE `players` SET `XUID` = ? WHERE `Name` = ?;",
        {xuid, Utils::strToLower(playerName)}
    );
}

bool BaseManager::updateRankNameByPlayerName(const std::string& playerName, const std::string& rankName) {
    return connectionPool->executeUpdateQuery(
        "UPDATE `players` SET `RankName` = ? WHERE `Name` = ?;",
        {rankName, Utils::strToLower(playerName)}
    );
}

bool BaseManager::updateRankNameByXuid(const std::string& xuid, const std::string& rankName) {
    return connectionPool->executeUpdateQuery(
        "UPDATE `players` SET `RankName` = ? WHERE `XUID` = ?;",
        {rankName, xuid}
    );
}

BaseManager* BaseManager::getInstance() { return instance; }

} // namespace power_ranks::manager