#include "mono_window_service/src/pch.h"
#include "mono_window_service/include/window_service_command_list.h"

#include "mono_window_service/include/window_service.h"
#include "window_provider/include/win32_window.h"

namespace mono_window_service
{

void WindowServiceCommandList::CreateWindowByService(
    window_provider::WindowHandle& out_window_handle,
    std::unique_ptr<window_provider::Window::CreateDesc> create_desc)
{
    AddCommand([&out_window_handle, desc = std::move(create_desc)](mono_service::ServiceAPI& api) mutable
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        bool result = false;

        // Instantiate a win32 window
        std::unique_ptr<window_provider::Win32Window> window
            = std::make_unique<window_provider::Win32Window>();

        // Create the window
        result = window->Create(std::move(desc));
        if (!result) 
            return false; // Creation failed

        // Add the window to the window container by using window adder
        out_window_handle = window_service_api.GetWindowAdder().AddWindow(std::move(window));
        if (!out_window_handle.IsValid())
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::DestroyWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Destroy the window via window eraser
        window_service_api.GetWindowEraser().EraseWindow(window_handle);

        return true; // Success
    });
}

void WindowServiceCommandList::ShowWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get window from this handle
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Show the window
        bool result = window.Show();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::HideWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get window from this handle
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Hide the window
        bool result = window.Hide();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::ResizeWindowByService(
    const window_provider::WindowHandle& window_handle, uint32_t width, uint32_t height)
{
    AddCommand([window_handle, width, height](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get window from this handle
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Resize the window
        bool result = window.Resize(width, height);
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::MaximizeWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get the window from this handle
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Maximize the window
        bool result = window.Maximize();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::MinimizeWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get the window
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Minimize the window
        bool result = window.Minimize();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::RestoreWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get the window
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Restore the window
        bool result = window.Restore();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

void WindowServiceCommandList::FullscreenWindowByService(
    const window_provider::WindowHandle& window_handle)
{
    AddCommand([window_handle](mono_service::ServiceAPI& api)
    {
        // Get window service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
            "WindowServiceAPI must be derived from ServiceAPI.");
        mono_window_service::WindowServiceAPI& window_service_api 
            = dynamic_cast<mono_window_service::WindowServiceAPI&>(api);

        // Get the window
        window_provider::Window& window = window_service_api.GetWindow(window_handle);

        // Fullscreen the window
        bool result = window.Fullscreen();
        if (!result)
            return false; // Failure

        return true; // Success
    });
}

} // namespace mono_window_service