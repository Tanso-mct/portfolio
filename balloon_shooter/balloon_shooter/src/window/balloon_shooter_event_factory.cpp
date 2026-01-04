#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/window/balloon_shooter_event_factory.h"

#include "balloon_shooter/include/window/balloon_shooter_window.h"
#include "balloon_shooter/include/window/balloon_shooter_monitor_keyboard.h"
#include "balloon_shooter/include/window/balloon_shooter_monitor_mouse.h"

#include "balloon_shooter/include/scene/play_scene.h"
#include "balloon_shooter/include/scene/title_scene.h"

std::unique_ptr<wb::IWindowEvent> balloon_shooter::BalloonShooterWindowEventFactory::Create() const
{
    std::unique_ptr<wb::IWindowEvent> event = std::make_unique<wb::DefaultWindowEvent>();

    // Set the window ID and other necessary properties
    event->SetWindowID(balloon_shooter::BalloonShooterWindowID());

    {
        std::unique_ptr<wb::ISceneUpdator> sceneUpdator = std::make_unique<wb::SceneUpdator>();
        sceneUpdator->SetBelongWindowID(balloon_shooter::BalloonShooterWindowID());
        sceneUpdator->SetInitialSceneID(balloon_shooter::TitleSceneFacadeID());

        event->SetSceneUpdator(std::move(sceneUpdator));
    }

    event->SetKeyboardMonitorID(balloon_shooter::BalloonShooterKeyboardMonitorID());
    event->SetMouseMonitorID(balloon_shooter::BalloonShooterMouseMonitorID());

    return event;
}