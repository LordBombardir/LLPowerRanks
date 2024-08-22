#pragma once

#include <ll/api/form/CustomForm.h>
#include <ll/api/form/FormBase.h>

namespace power_ranks::forms {

class AddRankForm {
public:
    static void init(Player& player);
    static void handle(Player& player, const ll::form::CustomFormResult& result, ll::form::FormCancelReason reason);
};

} // namespace power_ranks::forms