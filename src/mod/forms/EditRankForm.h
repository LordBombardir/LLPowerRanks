#pragma once

#include "../types/Rank.h"
#include <ll/api/form/CustomForm.h>
#include <ll/api/form/FormBase.h>

namespace power_ranks::forms {

class EditRankForm {
public:
    static void init(Player& player, types::Rank* rank);
};

} // namespace power_ranks::forms