#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class TitleSystemScheduler : public wb::ISystemScheduler
    {
    public:
        TitleSystemScheduler() = default;
        ~TitleSystemScheduler() override = default;

        void Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args) override;
    };

} // namespace balloon_shooter