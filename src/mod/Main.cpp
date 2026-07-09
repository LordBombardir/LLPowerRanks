#include "Main.h"
#include "core/MainManager.h"
#include "lang/LanguageManager.h"

#include <ll/api/mod/RegisterHelper.h>

namespace power_ranks {

Main& Main::getInstance() {
    static Main instance;
    return instance;
}

bool Main::load() {
    getSelf().getLogger().info("The mod is loading...");

    if (!MainManager::initModWhileLoading(getSelf())) {
        getSelf().getLogger().error("Failed to load the mod!");
        return false;
    }

    getSelf().getLogger().info("The mod has been successfully loaded!");
    return true;
}

bool Main::enable() {
    getSelf().getLogger().info("The mod is enabling...");

    if (!MainManager::initModWhileEnabling(getSelf())) {
        getSelf().getLogger().error("Failed to enable the mod!");
        return false;
    }

    getSelf().getLogger().info(
        "The mod has been successfully enabled! Chosen language: " + LanguageManager::getTranslate("languageName")
    );

    getSelf().getLogger().info("Author: vk.com/lordbomba");
    return true;
}

bool Main::disable() {
    getSelf().getLogger().info("The mod is disabling...");

    MainManager::disableMod();

    getSelf().getLogger().info("The mod has been successfully disabled.");
    return true;
}

} // namespace power_ranks

LL_REGISTER_MOD(power_ranks::Main, power_ranks::Main::getInstance());
