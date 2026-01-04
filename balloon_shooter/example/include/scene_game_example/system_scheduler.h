#pragma once
#include "windows_base/windows_base.h"

namespace example
{
    class GameExampleSystemScheduler : public wb::ISystemScheduler
    {
    public:
        GameExampleSystemScheduler() = default;
        ~GameExampleSystemScheduler() override = default;

        void Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args) override;
    };

} // namespace example