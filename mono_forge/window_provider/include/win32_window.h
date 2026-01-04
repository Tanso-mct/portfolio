#pragma once

#include <string>
#include <windows.h>

#include "window_provider/include/dll_config.h"
#include "window_provider/include/window.h"

namespace window_provider
{

namespace win32
{

// Win32 window default variables
constexpr const wchar_t* WINDOW_DEFAULT_NAME = L"Win32WindowClass";
constexpr uint32_t WINDOW_DEFAULT_WIDTH = 800;
constexpr uint32_t WINDOW_DEFAULT_HEIGHT = 600;

// Win32 show state enumeration
enum class WindowShowState
{
    HIDE,
    SHOW,
    MAXIMIZE,
    MINIMIZE,
    RESTORE,
    FULLSCREEN
};

} // namespace win32

class WINDOW_PROVIDER_DLL Win32Window : 
    public Window
{
public:
    Win32Window() = default;
    ~Win32Window() override;

    // The descriptor for creating a win32 window
    class CreateDesc : 
        public Window::CreateDesc
    {
    public:
        CreateDesc() :
            title(win32::WINDOW_DEFAULT_NAME),
            window_style(WS_OVERLAPPEDWINDOW),
            width(win32::WINDOW_DEFAULT_WIDTH),
            height(win32::WINDOW_DEFAULT_HEIGHT),
            class_name(win32::WINDOW_DEFAULT_NAME),
            show_state(win32::WindowShowState::SHOW)
        {
        }

        ~CreateDesc() override = default;

        // The window class information
        WNDCLASSEX wc = {};

        // The window style
        DWORD window_style;

        // Window title
        std::wstring title;

        // Window width
        uint32_t width;

        // Window height
        uint32_t height;

        // Class name
        std::wstring class_name;

        // Initial show state
        win32::WindowShowState show_state;
    };
    
    bool Create(std::unique_ptr<Window::CreateDesc> desc) override;
    void Destroy() override;
    bool Show() override;
    bool Hide() override;
    bool Resize(uint32_t width, uint32_t height) override;
    bool Maximize() override;
    bool Minimize() override;
    bool Restore() override;
    bool Fullscreen() override;
    uint32_t GetWidth() const override;
    uint32_t GetHeight() const override;
    uint32_t GetClientWidth() const override;
    uint32_t GetClientHeight() const override;
    uint32_t GetPositionX() const override;
    uint32_t GetPositionY() const override;

    // Get the native Win32 window handle
    HWND GetHwnd() const { return hwnd_; }

private:
    // The handle to the Win32 window
    HWND hwnd_ = nullptr;

    // The descriptor used to create the window
    std::unique_ptr<Window::CreateDesc> desc_ = nullptr;
};

} // namespace window_provider