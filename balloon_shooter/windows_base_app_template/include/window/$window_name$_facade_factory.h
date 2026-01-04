#pragma once
#include "windows_base/windows_base.h"

namespace $project_name$
{
    class $WindowName$WindowFacadeFactory : public wb::IWindowFacadeFactory
    {
    public:
        $WindowName$WindowFacadeFactory() = default;
        ~$WindowName$WindowFacadeFactory() override = default;

        std::unique_ptr<wb::IWindowFacade> Create() const override;
    };

} // namespace $project_name$