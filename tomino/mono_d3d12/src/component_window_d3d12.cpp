#include "mono_d3d12/src/pch.h"
#include "mono_d3d12/include/component_window_d3d12.h"

#include "mono_d3d12/include/system_window_d3d12.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

mono_d3d12::WindowD3D12State::WindowD3D12State()
{
}

mono_d3d12::WindowD3D12State::~WindowD3D12State()
{
}

mono_d3d12::WindowD3D12Action::WindowD3D12Action()
{
}

mono_d3d12::WindowD3D12Action::~WindowD3D12Action()
{
}

mono_d3d12::ComponentWindowD3D12::ComponentWindowD3D12()
{
}

mono_d3d12::ComponentWindowD3D12::~ComponentWindowD3D12()
{
    info_.~WindowD3D12Info();
    state_.~WindowD3D12State();
    action_.~WindowD3D12Action();

    keyboardState_.~KeyboardInputState();
    mouseState_.~MouseInputState();

    sceneEntity_ = riaecs::Entity();
}

void mono_d3d12::ComponentWindowD3D12::Setup(const ComponentWindowD3D12::SetupParam &param)
{
    if (state_.IsCreated())
        riaecs::NotifyError({"Cannot setup a window that is already created."}, RIAECS_LOG_LOC);

    info_.SetName(param.windowName_);
    info_.SetItClassName(param.windowClassName_);
    info_.SetWidth(param.windowWidth_);
    info_.SetHeight(param.windowHeight_);
    info_.SetPosX(param.windowPosX_);
    info_.SetPosY(param.windowPosY_);
    sceneEntity_ = param.sceneEntity_;

    action_.SetNeedsQuitAppWhenDestroyed(param.needsQuitAppWhenDestroyed_);
}

void mono_d3d12::ComponentWindowD3D12::Create()
{
    if (state_.IsCreated())
        riaecs::NotifyError({"This window is already created."}, RIAECS_LOG_LOC);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = mono_d3d12::WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = info_.GetItClassName().c_str();
    if (!RegisterClassEx(&wc))
    {
        DWORD err = GetLastError();
        riaecs::NotifyError({"Failed to register window class."}, RIAECS_LOG_LOC);
    }

    HWND handle = CreateWindow
    (
        info_.GetItClassName().c_str(), info_.GetName().c_str(), info_.GetStyle(),
        info_.GetPosX(), info_.GetPosY(), 
		static_cast<int>(info_.GetWidth()),
		static_cast<int>(info_.GetHeight()),
        info_.GetParentHandle(), nullptr, wc.hInstance, nullptr
    );
    if (!handle)
    {
        DWORD err = GetLastError();
        riaecs::NotifyError({"Failed to create window."}, RIAECS_LOG_LOC);
    }

    info_.SetHandle(handle);
    info_.SetInstance(wc.hInstance);
    state_.SetCreated(true);

    if (!UpdateWindow(info_.GetHandle()))
        riaecs::NotifyError({"Failed to update window."}, RIAECS_LOG_LOC);

    // Update window size and position
    RECT windowRect;
    if (!GetWindowRect(info_.GetHandle(), &windowRect))
        riaecs::NotifyError({"Failed to get window rectangle."}, RIAECS_LOG_LOC);
    info_.SetWidth(windowRect.right - windowRect.left);
    info_.SetHeight(windowRect.bottom - windowRect.top);
    info_.SetPosX(windowRect.left);
    info_.SetPosY(windowRect.top);

    // Update client size
    RECT clientRect;
    if (!GetClientRect(info_.GetHandle(), &clientRect))
        riaecs::NotifyError({"Failed to get client rectangle."}, RIAECS_LOG_LOC);
    info_.SetClientWidth(clientRect.right - clientRect.left);
    info_.SetClientHeight(clientRect.bottom - clientRect.top);
}

void mono_d3d12::ComponentWindowD3D12::Destroy()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"This window is not created."}, RIAECS_LOG_LOC);

    if (!DestroyWindow(info_.GetHandle()))
        riaecs::NotifyError({"Failed to destroy window."}, RIAECS_LOG_LOC);

    if (!UnregisterClass(info_.GetItClassName().c_str(), info_.GetInstance()))
        riaecs::NotifyError({"Failed to unregister window class."}, RIAECS_LOG_LOC);

    info_.SetHandle(nullptr);
}

void mono_d3d12::ComponentWindowD3D12::Resize(UINT width, UINT height)
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before resizing."}, RIAECS_LOG_LOC);

    if (width == 0 || height == 0)
        riaecs::NotifyError({"Width and height must be greater than zero."}, RIAECS_LOG_LOC);

    info_.SetWidth(width);
    info_.SetHeight(height);

    if (!SetWindowPos(info_.GetHandle(), nullptr, info_.GetPosX(), info_.GetPosY(), width, height, SWP_NOZORDER))
        riaecs::NotifyError({"Failed to set window size."}, RIAECS_LOG_LOC);

}

void mono_d3d12::ComponentWindowD3D12::Show()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before showing."}, RIAECS_LOG_LOC);

    ShowWindow(info_.GetHandle(), SW_SHOW);
}

void mono_d3d12::ComponentWindowD3D12::Hide()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before hiding."}, RIAECS_LOG_LOC);

    ShowWindow(info_.GetHandle(), SW_HIDE);
}

void mono_d3d12::ComponentWindowD3D12::Focus()
{
    Show();

    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before focusing."}, RIAECS_LOG_LOC);

    if (!SetForegroundWindow(info_.GetHandle()))
        riaecs::NotifyError({"Failed to set window to foreground."}, RIAECS_LOG_LOC);

    if (!SetFocus(info_.GetHandle()))
        riaecs::NotifyError({"Failed to set focus to window."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::UnFocus()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before unfocusing."}, RIAECS_LOG_LOC);

    if (!SetFocus(nullptr))
        riaecs::NotifyError({"Failed to remove focus from window."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::Maximize()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before maximizing."}, RIAECS_LOG_LOC);

    if (!ShowWindow(info_.GetHandle(), SW_MAXIMIZE))
        riaecs::NotifyError({"Failed to maximize window."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::Minimize()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before minimizing."}, RIAECS_LOG_LOC);

    if (!ShowWindow(info_.GetHandle(), SW_MINIMIZE))
        riaecs::NotifyError({"Failed to minimize window."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::FullScreen()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before full screening."}, RIAECS_LOG_LOC);

    if (!SetWindowLong(info_.GetHandle(), GWL_STYLE, WS_POPUP | WS_VISIBLE))
        riaecs::NotifyError({"Failed to set window style for full screen."}, RIAECS_LOG_LOC);

    MONITORINFO mi = {0};
    HMONITOR hMonitor = MonitorFromWindow(info_.GetHandle(), MONITOR_DEFAULTTONEAREST);
    mi.cbSize = sizeof(mi);
    if (!GetMonitorInfo(hMonitor, &mi))
        riaecs::NotifyError({"Failed to get monitor info."}, RIAECS_LOG_LOC);

    UINT monitorWidth = mi.rcMonitor.right - mi.rcMonitor.left;
    UINT monitorHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;

    BOOL result = SetWindowPos
    (
        info_.GetHandle(), nullptr, 0, 0,
        monitorWidth, monitorHeight, SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE
    );
    if (!result)
        riaecs::NotifyError({"Failed to set window to full screen."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::Restore()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before restoring."}, RIAECS_LOG_LOC);

    if (!SetWindowLong(info_.GetHandle(), GWL_STYLE, info_.GetStyle()))
        riaecs::NotifyError({"Failed to restore window style."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::SetPosition(UINT x, UINT y)
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before setting position."}, RIAECS_LOG_LOC);

    info_.SetPosX(x);
    info_.SetPosY(y);

    if (!SetWindowPos(info_.GetHandle(), nullptr, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER))
        riaecs::NotifyError({"Failed to set window position."}, RIAECS_LOG_LOC);
}

void mono_d3d12::ComponentWindowD3D12::OnDestroyed()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"This window is not created."}, RIAECS_LOG_LOC);

    info_.~WindowD3D12Info();
    state_.~WindowD3D12State();

    keyboardState_.~KeyboardInputState();
    mouseState_.~MouseInputState();
}

void mono_d3d12::ComponentWindowD3D12::OnResized()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before resizing."}, RIAECS_LOG_LOC);

    if (IsIconic(info_.GetHandle()))
        return;

    // Update window size and position
    RECT windowRect;
    if (!GetWindowRect(info_.GetHandle(), &windowRect))
        riaecs::NotifyError({"Failed to get window rectangle."}, RIAECS_LOG_LOC);
    info_.SetWidth(windowRect.right - windowRect.left);
    info_.SetHeight(windowRect.bottom - windowRect.top);
    info_.SetPosX(windowRect.left);
    info_.SetPosY(windowRect.top);

    // Update client size
    RECT clientRect;
    if (!GetClientRect(info_.GetHandle(), &clientRect))
        riaecs::NotifyError({"Failed to get client rectangle."}, RIAECS_LOG_LOC);
    info_.SetClientWidth(clientRect.right - clientRect.left);
    info_.SetClientHeight(clientRect.bottom - clientRect.top);
}

void mono_d3d12::ComponentWindowD3D12::OnFocused()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before focusing."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetFocused(true);
}

void mono_d3d12::ComponentWindowD3D12::OnUnFocused()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before unfocusing."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetFocused(false);
}

void mono_d3d12::ComponentWindowD3D12::OnMaximized()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before maximizing."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetMaximized(true);
}

void mono_d3d12::ComponentWindowD3D12::OnMinimized()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before minimizing."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetMinimized(true);
}

void mono_d3d12::ComponentWindowD3D12::OnFullScreened()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before full screening."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetFullScreened(true);
}

void mono_d3d12::ComponentWindowD3D12::OnRestored()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before restoring."}, RIAECS_LOG_LOC);

    // Update state
    state_.SetMaximized(false);
    state_.SetMinimized(false);
    state_.SetFullScreened(false);
}

void mono_d3d12::ComponentWindowD3D12::OnMoved()
{
    if (!state_.IsCreated())
        riaecs::NotifyError({"Window must be created before moving."}, RIAECS_LOG_LOC);

    // Update window position
    RECT windowRect;
    if (!GetWindowRect(info_.GetHandle(), &windowRect))
        riaecs::NotifyError({"Failed to get window rectangle."}, RIAECS_LOG_LOC);
    info_.SetPosX(windowRect.left);
    info_.SetPosY(windowRect.top);
}

MONO_D3D12_API riaecs::ComponentRegistrar
<mono_d3d12::ComponentWindowD3D12, mono_d3d12::ComponentWindowD3D12MaxCount> mono_d3d12::ComponentWindowD3D12ID;