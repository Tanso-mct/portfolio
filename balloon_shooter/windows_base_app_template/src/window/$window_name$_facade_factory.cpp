#include "$project_name$/src/pch.h"
#include "$project_name$/include/window/$window_name$_facade_factory.h"

#include "$project_name$/include/window/$window_name$_monitor_keyboard.h"
#include "$project_name$/include/window/$window_name$_monitor_mouse.h"

std::unique_ptr<wb::IWindowFacade> $project_name$::$WindowName$WindowFacadeFactory::Create() const
{
    std::unique_ptr<wb::IWindowFacade> facade = std::make_unique<wb::DefaultWindowFacade>();

    {
        std::unique_ptr<wb::IWindowContext> context = std::make_unique<wb::WindowContext>();
        context->Name() = L"$WindowName$ Window";

        facade->SetContext(std::move(context));
    }

    facade->AddMonitorID($project_name$::$WindowName$KeyboardMonitorID());
    facade->AddMonitorID($project_name$::$WindowName$MouseMonitorID());

    return facade;
}