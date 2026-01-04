#include "example/src/pch.h"
#include "example/include/window_game_example/monitor_keyboard.h"

const size_t &example::GameExampleKeyboardMonitorID()
{
    static size_t id = wb::IDFactory::CreateMonitorID();
    return id;
}

namespace example
{
    WB_REGISTER_MONITOR(GameExampleKeyboardMonitorID, wb::DefaultKeyboardMonitorFactoryID());
}