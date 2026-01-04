#include "window_provider_test/pch.h"

#include "window_provider/include/window_manager.h"
#include "window_provider/include/win32_window.h"

namespace window_provider_test
{

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
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

} // namespace window_provider_test

TEST(Win32Window, CreateAndDestroy)
{
    bool result = false;

    // Create window container
    std::unique_ptr<window_provider::WindowContainer> window_container 
        = std::make_unique<window_provider::WindowContainer>();

    // Create window manager
    std::unique_ptr<window_provider::WindowManager> window_manager 
        = std::make_unique<window_provider::WindowManager>(*window_container);

    // Create window adder
    std::unique_ptr<window_provider::WindowAdder> window_adder
        = std::make_unique<window_provider::WindowAdder>(*window_container);

    // Create window eraser
    std::unique_ptr<window_provider::WindowEraser> window_eraser
        = std::make_unique<window_provider::WindowEraser>(*window_container);

    // Instantiate a Win32Window
    std::unique_ptr<window_provider::Win32Window> win32_window
        = std::make_unique<window_provider::Win32Window>();

    // Add the window to the window container using the window adder
    window_provider::WindowHandle window_handle
        = window_adder->AddWindow(std::move(win32_window));
    ASSERT_TRUE(window_handle.IsValid());

    window_provider::WindowManager::GetInstance().WithLock([&](window_provider::WindowManager& manager)
    {
        // Get window by its handle
        window_provider::Window& window = manager.GetWindow(window_handle);

        const uint32_t WINDOW_WIDTH = 800;
        const uint32_t WINDOW_HEIGHT = 600;

        // Create CreateDesc for the window
        std::unique_ptr<window_provider::Win32Window::CreateDesc> create_desc
            = std::make_unique<window_provider::Win32Window::CreateDesc>();
        create_desc->wc = 
        { 
            sizeof(create_desc->wc), CS_CLASSDC, window_provider_test::WndProc, 0L, 0L,
            ::GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr,
            L"Win32WindowTestClass", nullptr 
        };
        create_desc->title = L"Win32 Window Test";
        create_desc->width = WINDOW_WIDTH;
        create_desc->height = WINDOW_HEIGHT;

        // Create the Win32 window
        result = window.Create(std::move(create_desc));
        ASSERT_TRUE(result);

        // Show the window
        result = window.Show();
        ASSERT_TRUE(result);

        // Check if the window exists
        ASSERT_TRUE(manager.Contains(window_handle));

        // Verify window properties
        ASSERT_EQ(window.GetWidth(), WINDOW_WIDTH);
        ASSERT_EQ(window.GetHeight(), WINDOW_HEIGHT);
    });   

    // Destroy the window using the window eraser
    window_eraser->EraseWindow(window_handle);

    // Verify that the window has been removed
    window_provider::WindowManager::GetInstance().WithLock([&](window_provider::WindowManager& manager)
    {
        // Check if the window exists
        ASSERT_FALSE(manager.Contains(window_handle));
    });

    // Cleanup
    window_eraser.reset();
    window_adder.reset();
    window_manager.reset();
    window_container.reset();
}

TEST(Win32Window, StageChange)
{
    bool result = false;

    // Create window container
    std::unique_ptr<window_provider::WindowContainer> window_container 
        = std::make_unique<window_provider::WindowContainer>();

    // Create window manager
    std::unique_ptr<window_provider::WindowManager> window_manager 
        = std::make_unique<window_provider::WindowManager>(*window_container);

    // Create window adder
    std::unique_ptr<window_provider::WindowAdder> window_adder
        = std::make_unique<window_provider::WindowAdder>(*window_container);

    // Create window eraser
    std::unique_ptr<window_provider::WindowEraser> window_eraser
        = std::make_unique<window_provider::WindowEraser>(*window_container);

    // Instantiate a Win32Window
    std::unique_ptr<window_provider::Win32Window> win32_window
        = std::make_unique<window_provider::Win32Window>();

    // Add the window to the window container using the window adder
    window_provider::WindowHandle window_handle
        = window_adder->AddWindow(std::move(win32_window));
    ASSERT_TRUE(window_handle.IsValid());

    window_provider::WindowManager::GetInstance().WithLock([&](window_provider::WindowManager& manager)
    {
        // Get window by its handle
        window_provider::Window& window = manager.GetWindow(window_handle);

        const uint32_t WINDOW_WIDTH = 800;
        const uint32_t WINDOW_HEIGHT = 600;

        // Create CreateDesc for the window
        std::unique_ptr<window_provider::Win32Window::CreateDesc> create_desc
            = std::make_unique<window_provider::Win32Window::CreateDesc>();
        create_desc->wc = 
        { 
            sizeof(create_desc->wc), CS_CLASSDC, window_provider_test::WndProc, 0L, 0L,
            ::GetModuleHandleW(nullptr), nullptr, nullptr, nullptr, nullptr,
            L"Win32WindowTestClass", nullptr 
        };
        create_desc->title = L"Win32 Window Test";
        create_desc->width = WINDOW_WIDTH;
        create_desc->height = WINDOW_HEIGHT;

        // Create the Win32 window
        result = window.Create(std::move(create_desc));
        ASSERT_TRUE(result);

        // Show the window
        result = window.Show();
        ASSERT_TRUE(result);

        // Resize the window
        result = window.Resize(100, 100);
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Maximize the window
        result = window.Maximize();
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Minimize the window
        result = window.Minimize();
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Fullscreen the window
        result = window.Fullscreen();
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Restore the window
        result = window.Restore();
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Hide the window
        result = window.Hide();
        ASSERT_TRUE(result);
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

        // Cast to Win32Window to access Win32-specific methods
        window_provider::Win32Window& win32_window
            = dynamic_cast<window_provider::Win32Window&>(window);
        HWND hwnd = win32_window.GetHwnd();
        ASSERT_TRUE(::IsWindow(hwnd));
    });   

    // Cleanup
    window_eraser.reset();
    window_adder.reset();
    window_manager.reset();
    window_container.reset();
}