#pragma once

#include <ll/api/mod/NativeMod.h>

namespace power_ranks {

class Main {

public:
    static Main& getInstance();

    Main(ll::mod::NativeMod& self) : mSelf(self) {}

    [[nodiscard]] ll::mod::NativeMod& getSelf() const { return mSelf; }

    [[maybe_unused]] bool load();
    [[maybe_unused]] bool enable();
    [[maybe_unused]] bool disable();

private:
    ll::mod::NativeMod& mSelf;
};

} // namespace power_ranks
