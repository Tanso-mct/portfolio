#include "$project_name$/src/pch.h"
#include "$project_name$/include/window/$window_name$_window.h"

#include "$project_name$/include/window/$window_name$_facade_factory.h"
#include "$project_name$/include/window/$window_name$_event_factory.h"

const size_t &$project_name$::$WindowName$WindowID()
{
    static size_t id = wb::IDFactory::CreateWindowID();
    return id;
}

namespace $project_name$
{
    WB_REGISTER_WINDOW($WindowName$WindowID, $WindowName$WindowFacadeFactory, $WindowName$WindowEventFactory);
}