#include "LanguageManager.h"
#include "../../commands/AddRankCommand.h"
#include "../../commands/EditRankCommand.h"
#include "../../commands/RemoveRankCommand.h"
#include "../../commands/SetRankCommand.h"
#include <memory>
#include <translator_api/Api.h>

namespace power_ranks::manager {

std::unique_ptr<ll::i18n::I18n> LanguageManager::i18n = nullptr;

void LanguageManager::init(ll::mod::NativeMod& mod) {
    i18n   = std::make_unique<ll::i18n::I18n>();
    auto _ = i18n->load(mod.getDataDir() / "languages");
}

std::string LanguageManager::getTranslate(const std::string_view& key, const std::string_view& localeCode) {
    return static_cast<std::string>(i18n->get(key, localeCode));
}

void LanguageManager::addTranslations() {
    translator::api::setPlaceholder(
        commands::AddRankCommand::getName(),
        manager::LanguageManager::getTranslate("commandAddRankDescription", "ru_RU"),
        "ru_RU"
    );
    translator::api::setPlaceholder(
        commands::EditRankCommand::getName(),
        manager::LanguageManager::getTranslate("commandEditRankDescription", "ru_RU"),
        "ru_RU"
    );
    translator::api::setPlaceholder(
        commands::RemoveRankCommand::getName(),
        manager::LanguageManager::getTranslate("commandRemoveRankDescription", "ru_RU"),
        "ru_RU"
    );
    translator::api::setPlaceholder(
        commands::SetRankCommand::getName(),
        manager::LanguageManager::getTranslate("commandSetRankDescription", "ru_RU"),
        "ru_RU"
    );
}

} // namespace power_ranks::manager