#include "LanguageManager.h"
#include "../../Utils.hpp"
#include <LLTranslatorApi.h>
#include <memory>

namespace power_ranks::manager {

std::unique_ptr<ll::i18n::I18n> LanguageManager::i18n = nullptr;

void LanguageManager::init(ll::mod::NativeMod& mod) {
    i18n = std::make_unique<ll::i18n::I18n>();
    i18n->load(Utils::fixPath(mod.getDataDir().string() + "/languages/"));
}

std::string LanguageManager::getTranslate(const std::string_view& key, const std::string_view& localeCode) {
    return static_cast<std::string>(i18n->get(key, localeCode));
}

void LanguageManager::addTranslations() {
    ::setTranslationForCommandDescription(
        "addrank",
        manager::LanguageManager::getTranslate("commandAddRankDescription", "ru_RU"),
        "ru_RU"
    );
    ::setTranslationForCommandDescription(
        "setrank",
        manager::LanguageManager::getTranslate("commandSetRankDescription", "ru_RU"),
        "ru_RU"
    );
    ::setTranslationForCommandDescription(
        "removerank",
        manager::LanguageManager::getTranslate("commandRemoveRankDescription", "ru_RU"),
        "ru_RU"
    );
    ::setTranslationForCommandDescription(
        "editrank",
        manager::LanguageManager::getTranslate("commandEditRankDescription", "ru_RU"),
        "ru_RU"
    );
}

} // namespace power_ranks::manager