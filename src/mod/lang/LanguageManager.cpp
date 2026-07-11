#include "LanguageManager.h"
#include "commands/types/AddRankCommand.h"
#include "commands/types/EditRankCommand.h"
#include "commands/types/RemoveRankCommand.h"
#include "commands/types/SetRankCommand.h"
#include "config/types/Config.h"

#include <memory>
#include <placeholder_api/Api.h>

namespace power_ranks {

std::unique_ptr<ll::i18n::I18n> LanguageManager::i18n = nullptr;

bool LanguageManager::init(ll::mod::NativeMod& mod) {
    i18n = std::make_unique<ll::i18n::I18n>();

    auto expected = i18n->load(mod.getDataDir() / "languages");
    if (!expected) {
        mod.getLogger().error("Failed to load translations: {}", expected.error().message());
        return false;
    }

    addTranslations(mod);
    return true;
}

std::string LanguageManager::getTranslate(std::string_view key, std::string_view localeCode) {
    return static_cast<std::string>(i18n->get(key, localeCode));
}

void LanguageManager::addTranslations(ll::mod::NativeMod& mod) {
    const auto& path = mod.getDataDir() / "languages";

    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (!entry.is_regular_file()) {
            continue;
        }

        const std::filesystem::path& file = entry.path();
        if (file.extension() != ".json") {
            continue;
        }

        const std::string localeCode = file.stem().string();
        if (localeCode == ConfigManager::getConfig().defaultLocaleCode) {
            continue;
        }

        placeholder::api::setPlaceholder(
            commands::AddRankCommand::getName(),
            LanguageManager::getTranslate("commandAddRankDescription", localeCode),
            localeCode
        );
        placeholder::api::setPlaceholder(
            commands::EditRankCommand::getName(),
            LanguageManager::getTranslate("commandEditRankDescription", localeCode),
            localeCode
        );
        placeholder::api::setPlaceholder(
            commands::RemoveRankCommand::getName(),
            LanguageManager::getTranslate("commandRemoveRankDescription", localeCode),
            localeCode
        );
        placeholder::api::setPlaceholder(
            commands::SetRankCommand::getName(),
            LanguageManager::getTranslate("commandSetRankDescription", localeCode),
            localeCode
        );
    }
}

} // namespace power_ranks
