#pragma once

#include "class_template/singleton.h"
#include "utility_header/container.h"

#include "window_provider/include/dll_config.h"
#include "window_provider/include/window_handle.h"
#include "window_provider/include/window.h"

namespace window_provider
{

// Type alias for window container
using WindowContainer = utility_header::Container<Window>;

// Singleton window manager class
class WINDOW_PROVIDER_DLL WindowManager :
    public class_template::Singleton<WindowManager>
{
public:
    WindowManager(WindowContainer& window_container)
        : window_container_(window_container)
    {
    }

    ~WindowManager() = default;

    // Execute a function with a unique lock on the window container
    template <typename Func>
    void WithLock(Func&& func)
    {
        window_container_.WithUniqueLock([&](WindowContainer& container)
        {
            func(*this);
        });
    }

    // Get a window by its handle
    Window& GetWindow(const WindowHandle& handle);

    // Get a const window by its handle
    const Window& GetWindow(const WindowHandle& handle) const;

    // Check if a window exists by its handle
    bool Contains(const WindowHandle& handle) const;
    
private:
    // Reference to the window container
    WindowContainer& window_container_;
};

// Class to add windows to the window container
class WINDOW_PROVIDER_DLL WindowAdder
{
public:
    WindowAdder(WindowContainer& window_container)
        : window_container_(window_container)
    {
    }

    ~WindowAdder() = default;

    // Add a new window and return its handle
    WindowHandle AddWindow(std::unique_ptr<Window> window);

private:
    // Reference to the window container
    WindowContainer& window_container_;
};

// Class to erase windows from the window container
class WINDOW_PROVIDER_DLL WindowEraser
{
public:
    WindowEraser(WindowContainer& window_container)
        : window_container_(window_container)
    {
    }

    ~WindowEraser() = default;

    // Erase a window by its handle
    void EraseWindow(const WindowHandle& handle);

private:
    // Reference to the window container
    WindowContainer& window_container_;
};


} // namespace window_provider