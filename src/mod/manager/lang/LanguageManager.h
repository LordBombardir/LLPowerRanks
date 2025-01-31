#pragma once

#include "../config/ConfigManager.h"
#include <ll/api/i18n/I18n.h>
#include <ll/api/mod/NativeMod.h>

namespace power_ranks::manager {

class LanguageManager final {
public:
    static void init(ll::mod::NativeMod& mod);

    static std::string getTranslate(
        const std::string_view& key,
        const std::string_view& localeCode = ConfigManager::getConfig().defaultLocaleCode
    );

    static void addTranslations();
private:
    LanguageManager()  = default;
    ~LanguageManager() = default;

    static std::unique_ptr<ll::i18n::I18n> i18n;
};

} // namespace power_ranks::manager