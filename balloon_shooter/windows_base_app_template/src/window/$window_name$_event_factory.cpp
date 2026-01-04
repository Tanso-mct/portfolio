#include "$project_name$/src/pch.h"
#include "$project_name$/include/$window_name$_event_factory.h"

#include "$project_name$/include/window/$window_name$_window.h"
#include "$project_name$/include/window/$window_name$_monitor_keyboard.h"
#include "$project_name$/include/window/$window_name$_monitor_mouse.h"

#include "$project_name$/include/scene/$scene_name$_scene.h"

std::unique_ptr<wb::IWindowEvent> $project_name$::$WindowName$WindowEventFactory::Create() const
{
    std::unique_ptr<wb::IWindowEvent> event = std::make_unique<wb::DefaultWindowEvent>();

    // Set the window ID and other necessary properties
    event->SetWindowID($project_name$::$WindowName$WindowID());

    {
        std::unique_ptr<wb::ISceneUpdator> sceneUpdator = std::make_unique<wb::SceneUpdator>();
        sceneUpdator->SetBelongWindowID($project_name$::$WindowName$WindowID());
        sceneUpdator->SetInitialSceneID($project_name$::$SceneName$SceneFacadeID());

        event->SetSceneUpdator(std::move(sceneUpdator));
    }

    event->SetKeyboardMonitorID($project_name$::$WindowName$KeyboardMonitorID());
    event->SetMouseMonitorID($project_name$::$WindowName$MouseMonitorID());

    return event;
}