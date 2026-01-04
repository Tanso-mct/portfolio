#include "render_graph_test/pch.h"

#include "imgui.h"
#include "imgui_impl_win32.h"

#include "render_graph/include/imgui_context_manager.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace imgui_test
{

// Window procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Window settings
constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 800;
constexpr int WINDOW_POS_X = 100;
constexpr int WINDOW_POS_Y = 100;

bool InitWindow(
    HWND& out_hwnd, float& out_main_scale, WNDCLASSEXW& out_wc, wchar_t* window_title, wchar_t* window_class_name)
{
    // Make process DPI aware and obtain main monitor scale
    ImGui_ImplWin32_EnableDpiAwareness();
    out_main_scale 
        = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

    // Register window class
    out_wc = 
    { 
        sizeof(out_wc), CS_CLASSDC, WndProc, 0L, 0L, 
        GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, window_class_name, nullptr 
    };
    if (!::RegisterClassExW(&out_wc))
        return false;

    // Create window
    out_hwnd = ::CreateWindowW
    (
        out_wc.lpszClassName, window_title, WS_OVERLAPPEDWINDOW, 
        WINDOW_POS_X, WINDOW_POS_Y, 
        static_cast<int>(WINDOW_WIDTH * out_main_scale), 
        static_cast<int>(WINDOW_HEIGHT * out_main_scale), 
        nullptr, nullptr, out_wc.hInstance, nullptr
    );
    if (out_hwnd == nullptr)
    {
        ::UnregisterClassW(out_wc.lpszClassName, out_wc.hInstance);
        return false;
    }

    ::ShowWindow(out_hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(out_hwnd);

    return true;
}

void CleanupWindow(HWND hwnd, WNDCLASSEXW& wc)
{
    ::DestroyWindow(hwnd);
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
}

} // namespace imgui_test

TEST(Imgui, ManageContext)
{
    bool result = false;

    // Create two different HWNDs for testing
    HWND hwnd1 = nullptr;
    float main_scale_1 = 1.0f;
    WNDCLASSEXW wc1 = {};
    result = imgui_test::InitWindow(hwnd1, main_scale_1, wc1, L"Imgui Test Window 1", L"ImguiTestWindowClass1");
    ASSERT_TRUE(result);

    HWND hwnd2 = nullptr;
    float main_scale_2 = 1.0f;
    WNDCLASSEXW wc2 = {};
    result = imgui_test::InitWindow(hwnd2, main_scale_2, wc2, L"Imgui Test Window 2", L"ImguiTestWindowClass2");
    ASSERT_TRUE(result);

    // Create ImguiContextContainer
    std::unique_ptr<render_graph::ImguiContextContainer> context_container 
        = std::make_unique<render_graph::ImguiContextContainer>();

    // Create singleton ImguiContextManager
    std::unique_ptr<render_graph::ImguiContextManager> context_manager 
        = std::make_unique<render_graph::ImguiContextManager>(*context_container);

    // Create ImguiContextAdder
    std::unique_ptr<render_graph::ImguiContextAdder> context_adder 
        = std::make_unique<render_graph::ImguiContextAdder>(*context_container);

    // Create ImguiContextEraser
    std::unique_ptr<render_graph::ImguiContextEraser> context_eraser 
        = std::make_unique<render_graph::ImguiContextEraser>(*context_container);

    // Add ImGui context for hwnd1
    render_graph::ImguiContextHandle handle1 = context_adder->Add(hwnd1, [](HWND hwnd)
    {
        ImGuiContext* context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);
        ImGui_ImplWin32_Init(hwnd);
        // NOTE: ImGui_ImplDX12_Init should be called here with appropriate parameters
        return context;
    }, 
    [](ImGuiContext* context)
    {
        ImGui::SetCurrentContext(context);
        ImGui_ImplWin32_Shutdown();
        // NOTE: ImGui_ImplDX12_Shutdown should be called here if it was initialized
        ImGui::DestroyContext(context);
    });

    // Add ImGui context for hwnd2
    render_graph::ImguiContextHandle handle2 = context_adder->Add(hwnd2, [](HWND hwnd)
    {
        ImGuiContext* context = ImGui::CreateContext();
        ImGui::SetCurrentContext(context);
        ImGui_ImplWin32_Init(hwnd);
        // NOTE: ImGui_ImplDX12_Init should be called here with appropriate parameters
        return context;
    }, 
    [](ImGuiContext* context)
    {
        ImGui::SetCurrentContext(context);
        ImGui_ImplWin32_Shutdown();
        // NOTE: ImGui_ImplDX12_Shutdown should be called here if it was initialized
        ImGui::DestroyContext(context);
    });

    // Get and verify ImGui context from context manager
    render_graph::ImguiContextManager::GetInstance().WithLock([&](render_graph::ImguiContextManager& manager)
    {
        render_graph::ImguiContext& context1 = manager.GetContext(handle1);
        ASSERT_NE(context1.Get(), nullptr);

        render_graph::ImguiContext& context2 = manager.GetContext(handle2);
        ASSERT_NE(context2.Get(), nullptr);

        ASSERT_NE(context1.Get(), context2.Get()); // Ensure different contexts
    });

    // Erase ImGui context for hwnd1
    context_eraser->Erase(handle1);

    // Verify that context1 is erased
    render_graph::ImguiContextManager::GetInstance().WithLock([&](render_graph::ImguiContextManager& manager)
    {
        ASSERT_FALSE(manager.Contains(handle1));
        ASSERT_TRUE(manager.Contains(handle2));
    });

    // Clean up
    context_manager.reset();
    context_adder.reset();
    context_eraser.reset();
    context_container.reset();

    imgui_test::CleanupWindow(hwnd1, wc1);
    imgui_test::CleanupWindow(hwnd2, wc2);
}