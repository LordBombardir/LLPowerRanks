#include "BaseManager.h"

namespace power_ranks::manager {

std::unique_ptr<base::pool::ConnectionPool> BaseManager::connectionPool = nullptr;
BaseManager*                                BaseManager::instance       = nullptr;

BaseManager::BaseManager(ll::mod::NativeMod& mod) {
    connectionPool = std::make_unique<base::pool::ConnectionPool>(
        base::pool::ConnectionPool((mod.getDataDir() / "base.db").generic_string())
    );
    connectionPool->setupDataBase([](sqlite3* db) -> void {
        char* errorMessage;

        std::string createTableQuery = "CREATE TABLE IF NOT EXISTS `ranks` "
                                       "("
                                       "`Name` TEXT DEFAULT 'null', "
                                       "`XUID` TEXT DEFAULT 'null', "
                                       "`RankName` TEXT NOT NULL"
                                       ");";

        sqlite3_exec(db, createTableQuery.c_str(), nullptr, nullptr, &errorMessage);
        sqlite3_free(errorMessage);
    });
}

void BaseManager::init(ll::mod::NativeMod& mod) { instance = new BaseManager(mod); }

void BaseManager::dispose() { delete getInstance(); }

std::optional<std::string> BaseManager::getPlayerRankByName(const std::string& playerName) {
    return connectionPool->executeSelectQuery("SELECT `RankName` FROM `ranks` WHERE `Name` = ?;", {playerName});
}

std::optional<std::string> BaseManager::getPlayerRankByXuid(const std::string& xuid) {
    return connectionPool->executeSelectQuery("SELECT `RankName` FROM `ranks` WHERE `XUID` = ?;", {xuid});
}

bool BaseManager::setPlayerRank(const std::string& playerName, const std::string& xuid, const std::string& rankName) {
    if (getPlayerRankByName(playerName).has_value() || getPlayerRankByXuid(xuid).has_value()) {
        return updateRankNameByPlayerName(playerName, rankName);
    }

    if (xuid.empty()) {
        return connectionPool->executeUpdateQuery(
            "INSERT INTO `ranks` (`Name`, `RankName`) VALUES (?, ?);",
            {playerName, rankName}
        );
    }

    return connectionPool->executeUpdateQuery(
        "INSERT INTO `ranks` (`Name`, `XUID`, `RankName`) VALUES (?, ?, ?);",
        {playerName, xuid, rankName}
    );
}

bool BaseManager::updatePlayerNameByXuid(const std::string& xuid, const std::string& playerName) {
    return connectionPool->executeUpdateQuery("UPDATE `ranks` SET `Name` = ? WHERE `XUID` = ?;", {playerName, xuid});
}

bool BaseManager::updateXuidByPlayerName(const std::string& playerName, const std::string& xuid) {
    return connectionPool->executeUpdateQuery("UPDATE `ranks` SET `XUID` = ? WHERE `Name` = ?;", {xuid, playerName});
}

bool BaseManager::updateRankNameByPlayerName(const std::string& playerName, const std::string& rankName) {
    return connectionPool->executeUpdateQuery(
        "UPDATE `ranks` SET `RankName` = ? WHERE `Name` = ?;",
        {rankName, playerName}
    );
}

bool BaseManager::updateRankNameByXuid(const std::string& xuid, const std::string& rankName) {
    return connectionPool->executeUpdateQuery("UPDATE `ranks` SET `RankName` = ? WHERE `XUID` = ?;", {rankName, xuid});
}

BaseManager* BaseManager::getInstance() { return instance; }

} // namespace power_ranks::manager