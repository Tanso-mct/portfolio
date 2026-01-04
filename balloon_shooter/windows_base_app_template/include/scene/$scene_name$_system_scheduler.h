#pragma once
#include "windows_base/windows_base.h"

namespace $project_name$
{
    class $SceneName$SystemScheduler : public wb::ISystemScheduler
    {
    public:
        $SceneName$SystemScheduler() = default;
        ~$SceneName$SystemScheduler() override = default;

        void Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args) override;
    };

} // namespace $project_name$