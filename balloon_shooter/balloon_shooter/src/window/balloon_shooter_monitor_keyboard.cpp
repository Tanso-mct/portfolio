#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/window/balloon_shooter_monitor_keyboard.h"

const size_t &balloon_shooter::BalloonShooterKeyboardMonitorID()
{
    static size_t id = wb::IDFactory::CreateMonitorID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_MONITOR(BalloonShooterKeyboardMonitorID, wb::DefaultKeyboardMonitorFactoryID());
}