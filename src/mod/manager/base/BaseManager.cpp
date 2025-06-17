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
                                       "`UUID` TEXT UNIQUE NOT NULL, "
                                       "`RankName` TEXT NOT NULL"
                                       ");";

        sqlite3_exec(db, createTableQuery.c_str(), nullptr, nullptr, &errorMessage);
        sqlite3_free(errorMessage);
    });
}

void BaseManager::init(ll::mod::NativeMod& mod) { instance = new BaseManager(mod); }

void BaseManager::dispose() { delete getInstance(); }

std::optional<std::string> BaseManager::getPlayerRank(const mce::UUID& uuid) {
    return connectionPool->executeSelectQuery("SELECT `RankName` FROM `ranks` WHERE `UUID` = ?;", {uuid.asString()});
}

bool BaseManager::setPlayerRank(const mce::UUID& uuid, const std::string& rankName) {
    return connectionPool->executeUpdateQuery(
        "INSERT INTO `ranks` (`UUID`, `RankName`) VALUES (?, ?) "
        "ON CONFLICT(`UUID`) DO UPDATE SET `RankName` = excluded.`RankName`;",
        {uuid.asString(), rankName}
    );
}

BaseManager* BaseManager::getInstance() { return instance; }

} // namespace power_ranks::manager