#pragma once
#include "windows_base/windows_base.h"

namespace balloon_shooter
{
    class LoadSystemScheduler : public wb::ISystemScheduler
    {
    public:
        LoadSystemScheduler() = default;
        ~LoadSystemScheduler() override = default;

        void Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args) override;
    };

} // namespace balloon_shooter