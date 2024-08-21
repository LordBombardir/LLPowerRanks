#pragma once

#include "../config/ConfigManager.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>

namespace power_ranks::manager {

class LanguageManager final {
public:
    static void init(ll::mod::NativeMod& mod);
    static void dispose();

    std::string getTranslate(
        const std::string_view& key,
        const std::string_view& localeName = ConfigManager::getConfig().defaultLocaleName
    );

    static LanguageManager* getInstance();

private:
    LanguageManager(ll::mod::NativeMod& mod);
    ~LanguageManager() = default;

    static std::unique_ptr<ll::i18n::I18N> i18n;
    static LanguageManager*                instance;
};

} // namespace power_ranks::manager