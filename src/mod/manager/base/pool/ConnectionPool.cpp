#include "ConnectionPool.h"
#include <stdexcept>

namespace power_ranks::manager::base::pool {

ConnectionPool::ConnectionPool(const std::string& pathToDataBase) : fileName(pathToDataBase) {}

void ConnectionPool::setupDataBase(std::function<void(sqlite3*)>&& setupFunction) {
    sqlite3* db = openConnection();
    setupFunction(db);

    closeConnection(db);
    dbIsSetup = true;
}

bool ConnectionPool::executeUpdateQuery(const std::string& sql, const std::vector<std::string>& params) {
    if (!dbIsSetup) {
        throw std::runtime_error("Failed to open database.");
    }

    sqlite3* db = openConnection();

    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        closeConnection(db);
        throw std::runtime_error("Failed to prepare SQL statement.");
    }

    for (int i = 0; i < static_cast<int>(params.size()); ++i) {
        sqlite3_bind_text(statement, i + 1, params[i].c_str(), -1, SQLITE_STATIC);
    }

    bool success = sqlite3_step(statement) == SQLITE_DONE;
    sqlite3_finalize(statement);
    closeConnection(db);

    return success;
}

std::optional<std::string>
ConnectionPool::executeSelectQuery(const std::string& sql, const std::vector<std::string>& params) {
    if (!dbIsSetup) {
        throw std::runtime_error("Failed to open database.");
    }

    sqlite3* db = openConnection();

    sqlite3_stmt* statement;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &statement, nullptr) != SQLITE_OK) {
        closeConnection(db);
        throw std::runtime_error("Failed to prepare SQL statement.");
    }

    for (int i = 0; i < static_cast<int>(params.size()); ++i) {
        sqlite3_bind_text(statement, i + 1, params[i].c_str(), -1, SQLITE_STATIC);
    }

    std::optional<std::string> result;
    if (sqlite3_step(statement) == SQLITE_ROW) {
        result = reinterpret_cast<const char*>(sqlite3_column_text(statement, 0));
    }

    sqlite3_finalize(statement);
    closeConnection(db);

    return result;
}

sqlite3* ConnectionPool::openConnection() {
    sqlite3* db;
    if (sqlite3_open_v2(fileName.c_str(), &db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database.");
    }
    return db;
}

void ConnectionPool::closeConnection(sqlite3* db) { sqlite3_close(db); }

} // namespace regions::base::pool