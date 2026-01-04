#include "mono_d3d12/src/pch.h"
#include "mono_d3d12/include/system_window_d3d12.h"

#include "mono_d3d12/include/component_window_d3d12.h"
#include "mono_d3d12/include/window_message_state.h"

#pragma comment(lib, "mono_input_monitor.lib")

namespace mono_d3d12
{
    MONO_D3D12_API ImGuiContext* g_HandleImguiContext = nullptr;

    LRESULT MONO_D3D12_API CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
    {
        ImGui::SetCurrentContext(g_HandleImguiContext);

        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return 0;

        mono_d3d12::WindowMessageState& messageState = mono_d3d12::WindowMessageState::GetInstance();
        messageState.AddMessage(hWnd, msg, wParam, lParam);

        switch (msg)
        {
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
                return 0;
            break;

        }

        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

} // namespace mono_d3d12

mono_d3d12::SystemWindowD3D12::SystemWindowD3D12()
{
}

mono_d3d12::SystemWindowD3D12::~SystemWindowD3D12()
{
}

bool mono_d3d12::SystemWindowD3D12::Update
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    for (const riaecs::Entity &entity : ecsWorld.View(mono_d3d12::ComponentWindowD3D12ID())())
    {
        mono_d3d12::ComponentWindowD3D12* window
        = riaecs::GetComponent<mono_d3d12::ComponentWindowD3D12>(ecsWorld, entity, mono_d3d12::ComponentWindowD3D12ID());

        /*******************************************************************************************************************
         * Update keyboard and mouse state
        /******************************************************************************************************************/

        mono_input_monitor::UpdateInputState(window->keyboardState_);
        mono_input_monitor::UpdateInputState(window->mouseState_);

        /***************************************************************************************************************
         * Process messages received by WindowProc
        /**************************************************************************************************************/

        mono_d3d12::WindowMessageState& messageState = mono_d3d12::WindowMessageState::GetInstance();
        std::vector<mono_d3d12::WindowMessage> windowMessages = messageState.TakeMessages(window->info_.GetHandle());

        for (const mono_d3d12::WindowMessage& windowMessage : windowMessages)
        {
            switch (windowMessage.message)
            {
            case WM_KEYDOWN:
            case WM_SYSKEYDOWN:
            case WM_KEYUP:
            case WM_SYSKEYUP:
                mono_input_monitor::EditInputState
                (
                    window->keyboardState_, 
                    keyInputConverter_.Convert(windowMessage.message), 
                    keyCodeConverter_.Convert(windowMessage.wParam, windowMessage.lParam)
                );
                break;

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
                mono_input_monitor::EditInputState
                (
                    window->mouseState_, 
                    mouseInputConverter_.Convert(windowMessage.message), 
                    mouseCodeConverter_.Convert(windowMessage.message, windowMessage.wParam), 
                    windowMessage.wParam, windowMessage.lParam
                );
                break;

            case WM_DESTROY:
                window->OnDestroyed();
                if (window->action_.NeedsQuitAppWhenDestroyed())
                    return false; // Stop the system loop

                break;

            case WM_SETFOCUS:
                window->OnFocused();
                break;

            case WM_KILLFOCUS:
                window->OnUnFocused();
                break;

            case WM_SIZE:
                if (windowMessage.wParam == SIZE_MAXIMIZED) window->OnMaximized();
                else if (windowMessage.wParam == SIZE_MINIMIZED) window->OnMinimized();
                else if (windowMessage.wParam == SIZE_RESTORED) window->OnRestored();
                window->OnResized();

            case WM_MOVE:
                window->OnMoved();
                break;

            default:
                break;
            };
        }
    }

    return true; // Continue the system loop
}

MONO_D3D12_API riaecs::SystemFactoryRegistrar<mono_d3d12::SystemWindowD3D12> mono_d3d12::SystemWindowD3D12ID;