#include "window_provider/src/pch.h"
#include "window_provider/include/window_manager.h"

namespace window_provider
{

Window& WindowManager::GetWindow(const WindowHandle& handle)
{
    return window_container_.Get(handle);
}

const Window& WindowManager::GetWindow(const WindowHandle& handle) const
{
    return window_container_.Get(handle);
}

bool WindowManager::Contains(const WindowHandle& handle) const
{
    return window_container_.Contains(handle);
}

WindowHandle WindowAdder::AddWindow(std::unique_ptr<Window> window)
{
    WindowHandle handle = WindowHandle();
    window_container_.WithUniqueLock([&](WindowContainer& container)
    {
        // Add the window and get its handle
        handle = container.Add(std::move(window));
    });

    return handle; // Return the window handle
}

void WindowEraser::EraseWindow(const WindowHandle& handle)
{
    window_container_.WithUniqueLock([&](WindowContainer& container)
    {
        // Erase the window by its handle
        container.Erase(handle);
    });
}

} // namespace window_provider