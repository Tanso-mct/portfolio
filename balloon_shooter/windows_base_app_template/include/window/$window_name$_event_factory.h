#pragma once
#include "windows_base/windows_base.h"

namespace $project_name$
{
    class $WindowName$WindowEventFactory : public wb::IWindowEventFactory
    {
    public:
        $WindowName$WindowEventFactory() = default;
        ~$WindowName$WindowEventFactory() override = default;

        std::unique_ptr<wb::IWindowEvent> Create() const override;
    };

} // namespace $project_name$

