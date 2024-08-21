#include "LanguageManager.h"
#include "../../Utils.hpp"
#include <memory>

namespace power_ranks::manager {

std::unique_ptr<ll::i18n::I18N> LanguageManager::i18n     = nullptr;
LanguageManager*                LanguageManager::instance = nullptr;

LanguageManager::LanguageManager(ll::mod::NativeMod& mod) {
    i18n = std::make_unique<ll::i18n::MultiFileI18N>(ll::i18n::MultiFileI18N(
        Utils::fixPath(mod.getDataDir().string() + "/languages/"),
        ConfigManager::getConfig().defaultLocaleName
    ));
}

void LanguageManager::init(ll::mod::NativeMod& mod) { instance = new LanguageManager(mod); }
void LanguageManager::dispose() { delete getInstance(); }

std::string LanguageManager::getTranslate(const std::string_view& key, const std::string_view& localeName) {
    return static_cast<std::string>(i18n->get(key, localeName));
}

LanguageManager* LanguageManager::getInstance() { return instance; }

} // namespace power_ranks::manager