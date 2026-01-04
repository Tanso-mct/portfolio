#include "mono_service/src/pch.h"
#include "mono_window_service/include/window_service_view.h"

#include "window_provider/include/win32_window.h"

namespace mono_window_service
{

WindowServiceView::WindowServiceView(const mono_service::ServiceAPI &service_api) :
    mono_service::ServiceView(service_api)
{
}

uint32_t WindowServiceView::GetWindowWidth(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window width
    return window_service_api.GetWindow(window_handle).GetWidth();
}

uint32_t WindowServiceView::GetWindowHeight(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window height
    return window_service_api.GetWindow(window_handle).GetHeight();
}

uint32_t WindowServiceView::GetWindowClientWidth(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window client width
    return window_service_api.GetWindow(window_handle).GetClientWidth();
}

uint32_t WindowServiceView::GetWindowClientHeight(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window client height
    return window_service_api.GetWindow(window_handle).GetClientHeight();
}

uint32_t WindowServiceView::GetWindowPositionX(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window position X
    return window_service_api.GetWindow(window_handle).GetPositionX();
}

uint32_t WindowServiceView::GetWindowPositionY(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Return window position Y
    return window_service_api.GetWindow(window_handle).GetPositionY();
}

HWND WindowServiceView::GetWindowHwnd(const window_provider::WindowHandle& window_handle) const
{
    // Get window service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, WindowServiceAPI>::value,
        "WindowServiceAPI must be derived from ServiceAPI.");
    const WindowServiceAPI& window_service_api = dynamic_cast<const WindowServiceAPI&>(service_api_);

    // Get window
    const window_provider::Window& window =
        window_service_api.GetWindow(window_handle);

    // The window must be a Win32 window
    // Dynamic cast to Win32Window
    const window_provider::Win32Window& win32_window =
        dynamic_cast<const window_provider::Win32Window&>(window);

    // Return HWND
    return win32_window.GetHwnd();
}

} // namespace mono_window_service