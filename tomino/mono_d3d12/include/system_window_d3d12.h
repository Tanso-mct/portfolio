#pragma once
#include "mono_d3d12/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_input_monitor/input_monitor.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace mono_d3d12
{
    // Global handling imGui context pointer
    extern MONO_D3D12_API ImGuiContext* g_HandleImguiContext;

    LRESULT MONO_D3D12_API WINAPI WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    class MONO_D3D12_API SystemWindowD3D12 : public riaecs::ISystem
    {
    private:
        mono_input_monitor::KeyCodeConverter keyCodeConverter_;
        mono_input_monitor::KeyInputConverter keyInputConverter_;
        
        mono_input_monitor::MouseCodeConverter mouseCodeConverter_;
        mono_input_monitor::MouseInputConverter mouseInputConverter_;

    public:
        SystemWindowD3D12();
        ~SystemWindowD3D12() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern MONO_D3D12_API riaecs::SystemFactoryRegistrar<SystemWindowD3D12> SystemWindowD3D12ID;

} // namespace mono_d3d12