#include "Main.h"
#include "hooks/Hooks.h"
#include "manager/MainManager.h"
#include "manager/lang/LanguageManager.h"
#include <ll/api/mod/RegisterHelper.h>
#include <memory>

namespace power_ranks {

static std::unique_ptr<Main> instance;

Main& Main::getInstance() { return *instance; }

bool Main::load() {
    getSelf().getLogger().info("The mod is loading...");

    if (!manager::MainManager::initManagers(getSelf())) {
        getSelf().getLogger().info("Failed to load the mod!");
        return false;
    }

    hooks::Hooks::setupHooks();

    getSelf().getLogger().info("The mod has been successfully loaded!");
    return true;
}

bool Main::enable() {
    getSelf().getLogger().info("The mod is enabling...");

    if (!manager::MainManager::registerCommands()) {
        getSelf().getLogger().info("Failed to enable the mod!");
        return false;
    }

    getSelf().getLogger().info(
        "The mod has been successfully enabled! Choosed language: "
        + manager::LanguageManager::getInstance()->getTranslate("languageName")
    );

    getSelf().getLogger().info("Author: vk.com/lordbomba");
    return true;
}

bool Main::disable() {
    getSelf().getLogger().info("The mod is disabling...");

    manager::MainManager::disposeManagers();

    getSelf().getLogger().info("The mod has been successfully disabled.");
    return true;
}

} // namespace power_ranks

LL_REGISTER_MOD(power_ranks::Main, power_ranks::instance);
