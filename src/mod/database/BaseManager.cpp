#include "BaseManager.h"

namespace power_ranks {

std::unique_ptr<ll::data::KeyValueDB> BaseManager::database = {};

void BaseManager::init(ll::mod::NativeMod& mod) {
    const auto path = mod.getDataDir() / "base";
    database        = std::make_unique<ll::data::KeyValueDB>(path);
}

std::optional<std::string> BaseManager::getPlayerRank(const mce::UUID& uuid) { return database->get(uuid.asString()); }

bool BaseManager::setPlayerRank(const mce::UUID& uuid, const std::string& rankName) {
    return database->set(uuid.asString(), rankName);
}

} // namespace power_ranks
