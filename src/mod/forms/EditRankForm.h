#pragma once

#include "types/Rank.h"

#include <ll/api/form/CustomForm.h>

namespace power_ranks::forms {

class EditRankForm {
public:
    static void init(Player& player, types::Rank* rank, const std::string& localeCode);
};

} // namespace power_ranks::forms
