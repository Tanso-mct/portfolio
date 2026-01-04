#include "windows_base/src/pch.h"
#include "windows_base/include/window.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"
#include "windows_base/include/type_cast.h"
#include "windows_base/include/windows_helper.h"

#include "windows_base/include/monitor_registry.h"
#include "windows_base/include/interfaces/monitor_keyboard.h"
#include "windows_base/include/interfaces/monitor_mouse.h"

void wb::DefaultWindowFacade::SetContext(std::unique_ptr<IContext> context)
{
    if (context_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Context is already set for this window facade."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_ = wb::UniqueAs<IWindowContext>(context);
    if (context_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to cast context to IWindowContext."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

bool wb::DefaultWindowFacade::CheckIsReady() const
{
    if (context_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Context is not set for this window facade."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

const HWND &wb::DefaultWindowFacade::GetHandle() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->Handle();
}

std::wstring_view wb::DefaultWindowFacade::GetName() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->Name();
}

const UINT &wb::DefaultWindowFacade::GetClientWidth() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->ClientWidth();
}

const UINT &wb::DefaultWindowFacade::GetClientHeight() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->ClientHeight();
}

bool wb::DefaultWindowFacade::IsCreated() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsCreated();
}

bool wb::DefaultWindowFacade::NeedsResize() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->NeedsResize();
}

bool wb::DefaultWindowFacade::NeedsToQuitApp() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->NeedsToQuitApp();
}

bool wb::DefaultWindowFacade::NeedsShowWhenCreated() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->NeedsShowWhenCreated();
}

bool wb::DefaultWindowFacade::IsFocusing() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsFocus();
}

bool &wb::DefaultWindowFacade::IsFocused()
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsFocused();
}

bool &wb::DefaultWindowFacade::IsUnFocused()
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsUnFocused();
}

bool wb::DefaultWindowFacade::IsMaximizing() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsMaximized();
}

bool wb::DefaultWindowFacade::IsMinimizing() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsMinimized();
}

bool wb::DefaultWindowFacade::IsFullScreen() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return context_->IsFullScreen();
}

void wb::DefaultWindowFacade::AddMonitorID(size_t monitorID)
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    monitorIDs_.push_back(monitorID);

    // Get the monitor's factory ID
    const size_t &monitorFactoryID = wb::gMonitorRegistry.GetFactoryID(monitorID);

    // Add the monitor's factory ID to the map
    monitorTypeToIDMap_[monitorFactoryID] = monitorIDs_.size() - 1;
}

const std::vector<size_t> &wb::DefaultWindowFacade::GetMonitorIDs() const
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return monitorIDs_;
}

void wb::DefaultWindowFacade::Create(WNDCLASSEX &wc)
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Create window
    /******************************************************************************************************************/

    wb::CreateWindowWB
    (
        context_->Handle(), context_->Style(), context_->Name().c_str(),
        context_->PosX(), context_->PosY(), context_->Width(), context_->Height(),
        context_->ParentHandle(), wc
    );

    // Keep the window's instance
    context_->Instance() = wc.hInstance;

    /*******************************************************************************************************************
     * Get the window's client area size
    /******************************************************************************************************************/

    {
        RECT clientRect;
        if (!GetClientRect(context_->Handle(), &clientRect))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to get client rectangle for the window."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->ClientWidth() = clientRect.right - clientRect.left;
        context_->ClientHeight() = clientRect.bottom - clientRect.top;

        POINT clientPos = {clientRect.left, clientRect.top};
        if (!ClientToScreen(context_->Handle(), &clientPos))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to convert client position to screen coordinates."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->PosX() = clientPos.x;
        context_->PosY() = clientPos.y;
    }

    /*******************************************************************************************************************
     * Enable raw input for the window
    /******************************************************************************************************************/

    {
        RAWINPUTDEVICE rawInputDevice[2];

        // Mouse
        rawInputDevice[0].usUsagePage = 0x01; // Generic Desktop Controls
        rawInputDevice[0].usUsage = 0x02; // Mouse
        rawInputDevice[0].dwFlags = 0;
        rawInputDevice[0].hwndTarget = context_->Handle();

        // Keyboard
        rawInputDevice[1].usUsagePage = 0x01; // Generic Desktop
        rawInputDevice[1].usUsage = 0x06; // Keyboard
        rawInputDevice[1].dwFlags = 0;
        rawInputDevice[1].hwndTarget = context_->Handle();

        if (!RegisterRawInputDevices(rawInputDevice, 2, sizeof(RAWINPUTDEVICE)))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to register raw input devices for the window."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }
    }

    /*******************************************************************************************************************
     * Set the created flag
    /******************************************************************************************************************/

    context_->IsCreated() = true;
}

void wb::DefaultWindowFacade::Show()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot show."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    ShowWindow(context_->Handle(), SW_SHOW);
}

void wb::DefaultWindowFacade::Hide()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot hide."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    ShowWindow(context_->Handle(), SW_HIDE);
}

void wb::DefaultWindowFacade::Destroy()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!DestroyWindow(context_->Handle()))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to destroy the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!UnregisterClass(context_->Name().c_str(), context_->Instance()))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to unregister the window class."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::Destroyed()
{
    /*******************************************************************************************************************
     * Show taskbar
    /******************************************************************************************************************/

    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_SHOW);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Reset the created flag
    /******************************************************************************************************************/

    context_->IsCreated() = false;
}

void wb::DefaultWindowFacade::Resize(UINT width, UINT height)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    SetWindowPos
    (
        context_->Handle(),
        nullptr,
        context_->PosX(), context_->PosY(),
        width, height,
        SWP_NOZORDER | SWP_NOACTIVATE
    );

    context_->NeedsResize() = true;
}

void wb::DefaultWindowFacade::Resized()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Update the client area size
    /******************************************************************************************************************/

    {
        RECT clientRect;
        if (!GetClientRect(context_->Handle(), &clientRect))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to get client rectangle for the window after resizing."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->ClientWidth() = clientRect.right - clientRect.left;
        context_->ClientHeight() = clientRect.bottom - clientRect.top;

        // Ensure minimum size constraints
        if (context_->ClientWidth() < wb::MINIMUM_WINDOW_WIDTH)
        {
            context_->ClientWidth() = wb::MINIMUM_WINDOW_WIDTH;
        }
        if (context_->ClientHeight() < wb::MINIMUM_WINDOW_HEIGHT)
        {
            context_->ClientHeight() = wb::MINIMUM_WINDOW_HEIGHT;
        }
    }

    /*******************************************************************************************************************
     * Reset the resize flag
    /******************************************************************************************************************/

    context_->NeedsResize() = false;
}

void wb::DefaultWindowFacade::Focus()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!ShowWindow(context_->Handle(), SW_SHOW))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to show the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!SetForegroundWindow(context_->Handle()))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to set the window to foreground."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (SetFocus(context_->Handle()) == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to set focus to the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::Focused()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsFocus() = true;
    context_->IsFocused() = true;

    // If the window is fullscreen, hide the taskbar
    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_HIDE);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::UnFocused()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsFocus() = false;
    context_->IsFocused() = false;
    context_->IsUnFocused() = true;

    // Show taskbar when unfocused
    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_SHOW);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::Maximize()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!ShowWindow(context_->Handle(), SW_MAXIMIZE))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to maximize the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    SetWindowLong(context_->Handle(), GWL_STYLE, context_->Style() | WS_VISIBLE);
}

void wb::DefaultWindowFacade::Maximized()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsMaximized() = true;
    context_->IsMinimized() = false;
    context_->IsFullScreen() = false;

    context_->NeedsResize() = true;
}

void wb::DefaultWindowFacade::Minimize()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!ShowWindow(context_->Handle(), SW_MINIMIZE))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to minimize the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    SetWindowLong(context_->Handle(), GWL_STYLE, context_->Style() | WS_VISIBLE);
}

void wb::DefaultWindowFacade::Minimized()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsMaximized() = false;
    context_->IsMinimized() = true;
    context_->IsFullScreen() = false;

    context_->NeedsResize() = true;
}

void wb::DefaultWindowFacade::FullScreen()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    UINT monitorWidth = 0;
    UINT monitorHeight = 0;
    wb::GetMonitorSize(context_->Handle(), monitorWidth, monitorHeight);

    // Set the window style to fullscreen
    SetWindowLong(context_->Handle(), GWL_STYLE, WS_POPUP | WS_VISIBLE);

    SetWindowPos
    (
        context_->Handle(),
        nullptr,
        0, 0,
        monitorWidth, monitorHeight,
        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED
    );

    // Hide the taskbar
    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_HIDE);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::FullScreened()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsMaximized() = false;
    context_->IsMinimized() = false;
    context_->IsFullScreen() = true;

    context_->NeedsResize() = true;
}

void wb::DefaultWindowFacade::Restore()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    if (!ShowWindow(context_->Handle(), SW_RESTORE))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to restore the window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    SetWindowLong(context_->Handle(), GWL_STYLE, context_->Style() | WS_VISIBLE);

    // Show the taskbar
    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_SHOW);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

void wb::DefaultWindowFacade::Restored()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->IsMaximized() = false;
    context_->IsMinimized() = false;
    context_->IsFullScreen() = false;

    context_->NeedsResize() = true;
}

void wb::DefaultWindowFacade::Move(UINT posX, UINT posY)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    SetWindowPos
    (
        context_->Handle(),
        nullptr,
        posX, posY,
        context_->ClientWidth(), context_->ClientHeight(),
        SWP_NOZORDER | SWP_NOACTIVATE
    );
}

void wb::DefaultWindowFacade::Moved()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created. Cannot destroy."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Update the position of the window
    RECT windowRect;
    if (!GetWindowRect(context_->Handle(), &windowRect))
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to get window rectangle after moving."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    context_->PosX() = windowRect.left;
    context_->PosY() = windowRect.top;
}

void wb::DefaultWindowEvent::SetWindowID(size_t windowID)
{
    windowID_ = windowID;
    isWindowIDSet_ = true;
}

void wb::DefaultWindowEvent::SetSceneUpdator(std::unique_ptr<ISceneUpdator> sceneUpdator)
{
    if (sceneUpdator_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene updator is already set for this window event."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    sceneUpdator_ = std::move(sceneUpdator);
}

void wb::DefaultWindowEvent::SetKeyboardMonitorID(size_t keyboardMonitorID)
{
    if (isKeyboardMonitorIDSet_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Keyboard monitor ID is already set for this window event."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    keyboardMonitorID_ = keyboardMonitorID;
    isKeyboardMonitorIDSet_ = true;
}

void wb::DefaultWindowEvent::SetMouseMonitorID(size_t mouseMonitorID)
{
    if (isMouseMonitorIDSet_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Mouse monitor ID is already set for this window event."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    mouseMonitorID_ = mouseMonitorID;
    isMouseMonitorIDSet_ = true;
}

bool wb::DefaultWindowEvent::CheckIsReady() const
{
    if (!isWindowIDSet_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window ID is not set for this window event."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (sceneUpdator_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Scene updator is not set for this window event."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

void wb::DefaultWindowEvent::OnEvent(ContainerStorage &contStorage, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
    {
        if (isKeyboardMonitorIDSet_)
        {
            wb::IMonitorContainer &monitorCont = contStorage.GetContainer<wb::IMonitorContainer>();
            wb::IMonitor &monitor = monitorCont.Get(keyboardMonitorID_);
            monitor.EditState(msg, wParam, lParam);
        }

        break;
    }

    case WM_MOUSEMOVE:
    case WM_MOUSEWHEEL:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    {
        if (isMouseMonitorIDSet_)
        {
            wb::IMonitorContainer &monitorCont = contStorage.GetContainer<wb::IMonitorContainer>();
            wb::IMonitor &monitor = monitorCont.Get(mouseMonitorID_);
            monitor.EditState(msg, wParam, lParam);
        }

        break;
    }

    case WM_INPUT:
    {
        // Get the size of the raw input data
        UINT dwSize = 0;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
        std::unique_ptr<BYTE[]> lpb = std::make_unique<BYTE[]>(dwSize);

        if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb.get(), &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
        {
            RAWINPUT* raw = (RAWINPUT*)lpb.get();

            // Get the window facade
            wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
            wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

            wb::IMonitorContainer &monitorCont = contStorage.GetContainer<wb::IMonitorContainer>();
            for (const size_t &monitorID : windowFacade.GetMonitorIDs())
            {
                wb::IMonitor &monitor = monitorCont.Get(monitorID);
                monitor.UpdateRawInputState(raw);
            }
        }

        break;
    }

    case WM_SETFOCUS:
    {
        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        windowFacade.Focused();

        break;
    }

    case WM_KILLFOCUS:
    {
        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        windowFacade.UnFocused();

        break;
    }

    case WM_SIZE:
    {
        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        if (wParam == SIZE_MAXIMIZED)
        {
            windowFacade.Maximized();
        }
        else if (wParam == SIZE_MINIMIZED)
        {
            windowFacade.Minimized();
        }
        else if (wParam == SIZE_RESTORED)
        {
            windowFacade.Restored();
        }

        break;
    }

    case WM_MOVE:
    {
        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        windowFacade.Moved();

        break;
    }

    case WM_PAINT:
    {
        /***************************************************************************************************************
         * Update scene by scene updator
        /**************************************************************************************************************/

        if (sceneUpdator_->NeedToExit())
        {
            while (!sceneUpdator_->IsFinishedLoading())
            {
                // Wait for asynchronous read process
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            while (!sceneUpdator_->IsFinishedReleasing())
            {
                // Wait for asynchronous release process
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }

            // When a scene ends, the current scene is released synchronously
            sceneUpdator_->SyncReleaseCurrentScene
            (
                contStorage.GetContainer<IAssetContainer>(),
                contStorage.GetContainer<ISceneContainer>()
            );

            wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
            wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);
            windowFacade.Destroy();
            break;
        }

        if (sceneUpdator_->NeedToLoad() && !sceneUpdator_->IsLoading())
        {
            sceneUpdator_->SyncLoadNextScene
            (
                contStorage.GetContainer<IAssetContainer>(),
                contStorage.GetContainer<ISceneContainer>()
            );
        }

        if (sceneUpdator_->IsSwitching() && !sceneUpdator_->IsLoading())
        {
            // Loading is done asynchronously, so scene switching is not performed here.
            sceneUpdator_->AsyncLoadNextScene
            (
                contStorage.GetContainer<IAssetContainer>(),
                contStorage.GetContainer<ISceneContainer>()
            );
        }

        if (sceneUpdator_->IsLoading())
        {
            if (sceneUpdator_->IsFinishedLoading())
            {
                if (sceneUpdator_->IsSwitching())
                {
                    // Asynchronously releases the current scene when a scene switch is made
                    sceneUpdator_->SyncReleaseCurrentScene
                    (
                        contStorage.GetContainer<IAssetContainer>(),
                        contStorage.GetContainer<ISceneContainer>()
                    );
                }

                sceneUpdator_->MoveToNextScene();
            }
        }

        sceneUpdator_->UpdateCurrentScene(contStorage);

        /***************************************************************************************************************
         * Update monitors
        /**************************************************************************************************************/        

        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        for (const size_t &monitorID : windowFacade.GetMonitorIDs())
        {
            wb::IMonitorContainer &monitorCont = contStorage.GetContainer<wb::IMonitorContainer>();
            wb::IMonitor &monitor = monitorCont.Get(monitorID);
            monitor.UpdateState();
        }

        break;
    }

    case WM_CLOSE:
    {
        while (!sceneUpdator_->IsFinishedLoading())
        {
            // Wait for asynchronous read process
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        while (!sceneUpdator_->IsFinishedReleasing())
        {
            // Wait for asynchronous release process
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        // When a scene ends, the current scene is released synchronously
        sceneUpdator_->SyncReleaseCurrentScene
        (
            contStorage.GetContainer<IAssetContainer>(),
            contStorage.GetContainer<ISceneContainer>()
        );

        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);
        windowFacade.Destroy();

        break;
    }

    case WM_DESTROY:
    {
        // Get the window facade
        wb::IWindowContainer &windowCont = contStorage.GetContainer<wb::IWindowContainer>();
        wb::IWindowFacade &windowFacade = windowCont.Get(windowID_);

        bool needsToQuitApp = windowFacade.NeedsToQuitApp();
        windowFacade.Destroyed();
        
        if (needsToQuitApp)
        {
            PostQuitMessage(0);
        }

        break;
    }

    }
}