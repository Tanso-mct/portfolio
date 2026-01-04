#pragma once

#include "mono_service/include/service_command.h"

#include "mono_window_service/include/dll_config.h"
#include "mono_window_service/include/window_service.h"

namespace mono_window_service
{

// The graphics command list class
class MONO_WINDOW_SERVICE_DLL WindowServiceCommandList :
    public mono_service::ServiceCommandList
{
public:
    WindowServiceCommandList() = default;
    ~WindowServiceCommandList() override = default;

    // Create window by window service
    void CreateWindowByService(
        window_provider::WindowHandle& out_window_handle,
        std::unique_ptr<window_provider::Window::CreateDesc> create_desc);

    // Destroy window by window service
    void DestroyWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Show window by window service
    void ShowWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Hide window by window service
    void HideWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Resize window by window service
    void ResizeWindowByService(
        const window_provider::WindowHandle& window_handle, uint32_t width, uint32_t height);

    // Maximize window by window service
    void MaximizeWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Minimize window by window service
    void MinimizeWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Restore window by window service
    void RestoreWindowByService(
        const window_provider::WindowHandle& window_handle);

    // Fullscreen window by window service
    void FullscreenWindowByService(
        const window_provider::WindowHandle& window_handle);
};

} // namespace mono_window_service