#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class PlaySystemScheduler : public wb::ISystemScheduler
    {
    public:
        PlaySystemScheduler() = default;
        ~PlaySystemScheduler() override = default;

        void Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args) override;
    };

} // namespace balloon_shooter