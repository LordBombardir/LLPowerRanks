#pragma once

#include <functional>
#include <optional>
#include <sqlite3.h>
#include <string>
#include <vector>

namespace power_ranks::manager::base::pool {

class ConnectionPool final {
public:
    ConnectionPool(const std::string& pathToDataBase);

    void setupDataBase(std::function<void(sqlite3*)>&& setupFunction);

    bool                       executeUpdateQuery(const std::string& sql, const std::vector<std::string>& params);
    std::optional<std::string> executeSelectQuery(const std::string& sql, const std::vector<std::string>& params);

private:
    bool dbIsSetup = false;

    std::string fileName;

    sqlite3* openConnection();
    void     closeConnection(sqlite3* db);
};

} // namespace power_ranks::manager::base::pool