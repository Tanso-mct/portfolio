#include "window_provider/src/pch.h"
#include "window_provider/include/win32_window.h"

#include "utility_header/logger.h"

namespace window_provider
{

using namespace win32;

Win32Window::~Win32Window()
{
    // Destroy the window if it exists
    if (IsWindow(hwnd_))
        Destroy();
}

bool Win32Window::Create(std::unique_ptr<Window::CreateDesc> desc)
{
    // Move the descriptor to the member variable
    desc_ = std::move(desc);

    // Cast to Win32Window::CreateDesc
    CreateDesc* win32_desc = dynamic_cast<CreateDesc*>(desc_.get());
    assert(win32_desc != nullptr && "Invalid CreateDesc type for Win32Window");

    // Update window class name
    win32_desc->wc.lpszClassName = win32_desc->class_name.c_str();

    // Register window class
    if (!::RegisterClassExW(&win32_desc->wc))
    {
        utility_header::ConsoleLogErr(
            { "Failed to register window class." }, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Create window
    hwnd_ = ::CreateWindowW
    (
        win32_desc->wc.lpszClassName, win32_desc->title.c_str(), win32_desc->window_style,
        CW_USEDEFAULT, CW_USEDEFAULT, static_cast<int>(win32_desc->width), static_cast<int>(win32_desc->height),
        nullptr, nullptr, win32_desc->wc.hInstance, nullptr
    );
    if (hwnd_ == nullptr)
    {
        utility_header::ConsoleLogErr(
            { "Failed to create window." }, __FILE__, __LINE__, __FUNCTION__);
            
        ::UnregisterClassW(win32_desc->wc.lpszClassName, win32_desc->wc.hInstance);
        return false;
    }

    // Update window
    ::UpdateWindow(hwnd_);

    // Show window based on initial show state
    switch (win32_desc->show_state)
    {
    case WindowShowState::SHOW:
        Show();
        break;

    case WindowShowState::HIDE:
        Hide();
        break;
    
    case WindowShowState::MAXIMIZE:
        Maximize();
        break;

    case WindowShowState::MINIMIZE:
        Minimize();
        break;

    case WindowShowState::RESTORE:
        Restore();
        break;

    case WindowShowState::FULLSCREEN:
        Fullscreen();
        break;
    }

    return true; // Success
}

void Win32Window::Destroy()
{
    // Cast to Win32Window::CreateDesc
    CreateDesc* win32_desc = dynamic_cast<CreateDesc*>(desc_.get());
    assert(win32_desc != nullptr && "Invalid CreateDesc type for Win32Window");

    ::DestroyWindow(hwnd_);
    ::UnregisterClassW(win32_desc->wc.lpszClassName, win32_desc->wc.hInstance);
}

bool Win32Window::Show()
{
    ::ShowWindow(hwnd_, SW_SHOW);
    return true;
}

bool Win32Window::Hide()
{
    ::ShowWindow(hwnd_, SW_HIDE);
    return true;
}

bool Win32Window::Resize(uint32_t width, uint32_t height)
{
    return ::SetWindowPos(hwnd_, nullptr, 0, 0, static_cast<int>(width), static_cast<int>(height), SWP_NOMOVE) != 0;
}

bool Win32Window::Maximize()
{
    ::ShowWindow(hwnd_, SW_MAXIMIZE);
    return true;
}

bool Win32Window::Minimize()
{
    ::ShowWindow(hwnd_, SW_MINIMIZE);
    return true;
}

bool Win32Window::Restore()
{
    ::ShowWindow(hwnd_, SW_RESTORE);
    return true;
}

bool Win32Window::Fullscreen()
{
    // Get the monitor info
    HMONITOR hMonitor = ::MonitorFromWindow(hwnd_, MONITOR_DEFAULTTONEAREST);
    MONITORINFO mi = {};
    mi.cbSize = sizeof(MONITORINFO);
    ::GetMonitorInfoW(hMonitor, &mi);

    // Set window style to borderless
    ::SetWindowLongW(hwnd_, GWL_STYLE, WS_POPUP | WS_VISIBLE);

    // Set window position and size to cover the entire monitor
    ::SetWindowPos(
        hwnd_, HWND_TOP,
        mi.rcMonitor.left, mi.rcMonitor.top,
        mi.rcMonitor.right - mi.rcMonitor.left,
        mi.rcMonitor.bottom - mi.rcMonitor.top,
        SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    return true;
}

uint32_t Win32Window::GetWidth() const
{
    RECT rect = {};
    ::GetWindowRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.right - rect.left);
}

uint32_t Win32Window::GetHeight() const
{
    RECT rect = {};
    ::GetWindowRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.bottom - rect.top);
}

uint32_t Win32Window::GetClientWidth() const
{
    RECT rect = {};
    ::GetClientRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.right - rect.left);
}

uint32_t Win32Window::GetClientHeight() const
{
    RECT rect = {};
    ::GetClientRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.bottom - rect.top);
}

uint32_t Win32Window::GetPositionX() const
{
    RECT rect = {};
    ::GetWindowRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.left);
}

uint32_t Win32Window::GetPositionY() const
{
    RECT rect = {};
    ::GetWindowRect(hwnd_, &rect);
    return static_cast<uint32_t>(rect.top);
}

} // namespace window_provider

