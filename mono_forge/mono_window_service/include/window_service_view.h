#pragma once

#include <cstdint>
#include <Windows.h>

#include "mono_service/include/service_view.h"

#include "mono_window_service/include/dll_config.h"
#include "mono_window_service/include/window_service.h"

namespace mono_window_service
{

class MONO_WINDOW_SERVICE_DLL WindowServiceView :
    public mono_service::ServiceView
{
public:
    WindowServiceView(const mono_service::ServiceAPI& service_api);
    virtual ~WindowServiceView() override = default;

    // Get window width by window handle
    uint32_t GetWindowWidth(const window_provider::WindowHandle& window_handle) const;

    // Get window height by window handle
    uint32_t GetWindowHeight(const window_provider::WindowHandle& window_handle) const;

    // Get window client width by window handle
    uint32_t GetWindowClientWidth(const window_provider::WindowHandle& window_handle) const;

    // Get window client height by window handle
    uint32_t GetWindowClientHeight(const window_provider::WindowHandle& window_handle) const;

    // Get window position X by window handle
    uint32_t GetWindowPositionX(const window_provider::WindowHandle& window_handle) const;

    // Get window position Y by window handle
    uint32_t GetWindowPositionY(const window_provider::WindowHandle& window_handle) const;

    // Get HWND by window handle
    // The window must be a Win32 window
    HWND GetWindowHwnd(const window_provider::WindowHandle& window_handle) const;
};

} // namespace mono_window_service