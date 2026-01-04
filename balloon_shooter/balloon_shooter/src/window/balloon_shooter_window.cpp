#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/window/balloon_shooter_window.h"

#include "balloon_shooter/include/window/balloon_shooter_facade_factory.h"
#include "balloon_shooter/include/window/balloon_shooter_event_factory.h"

const size_t &balloon_shooter::BalloonShooterWindowID()
{
    static size_t id = wb::IDFactory::CreateWindowID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_WINDOW(BalloonShooterWindowID, BalloonShooterWindowFacadeFactory, BalloonShooterWindowEventFactory);
}