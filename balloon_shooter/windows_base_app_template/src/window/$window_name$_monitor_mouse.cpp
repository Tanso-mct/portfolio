#include "$project_name$/src/pch.h"
#include "$project_name$/include/window/$window_name$_monitor_mouse.h"

const size_t &$project_name$::$WindowName$MouseMonitorID()
{
    static size_t id = wb::IDFactory::CreateMonitorID();
    return id;
}

namespace $project_name$
{
    WB_REGISTER_MONITOR($WindowName$MouseMonitorID, wb::DefaultMouseMonitorFactoryID());
}