#include "example/src/pch.h"
#include "example/include/window_game_example/event_factory.h"

#include "example/include/window_game_example/window.h"
#include "example/include/window_game_example/monitor_keyboard.h"
#include "example/include/window_game_example/monitor_mouse.h"

#include "example/include/scene_game_example/scene.h"

std::unique_ptr<wb::IWindowEvent> example::GameExampleWindowEventFactory::Create() const
{
    std::unique_ptr<wb::IWindowEvent> event = std::make_unique<wb::DefaultWindowEvent>();

    // Set the window ID and other necessary properties
    event->SetWindowID(example::GameExampleWindowID());

    {
        std::unique_ptr<wb::ISceneUpdator> sceneUpdator = std::make_unique<wb::SceneUpdator>();
        sceneUpdator->SetBelongWindowID(example::GameExampleWindowID());
        sceneUpdator->SetInitialSceneID(example::GameExampleSceneFacadeID());

        event->SetSceneUpdator(std::move(sceneUpdator));
    }

    event->SetKeyboardMonitorID(example::GameExampleKeyboardMonitorID());
    event->SetMouseMonitorID(example::GameExampleMouseMonitorID());

    return event;
}