#include "example/src/pch.h"
#pragma comment(lib, "windows_base.lib")

#include "example/include/window_game_example/window.h"
#include "example/include/scene_game_example/scene.h"

static LRESULT CALLBACK WindowProcWithEvent(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    wb::WindowsBaseLibrary::HandleWindowProc(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_PAINT:
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

int APIENTRY wWinMain
(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
){
    wb::LibraryConfig config;
    config.windowProc_ = WindowProcWithEvent;
    config.createWindowIDs_ = { example::GameExampleWindowID() };
    config.createSceneIDs_ = { example::GameExampleSceneFacadeID() };

    // Initialize the Windows Base Library
    wb::WindowsBaseLibrary::Initialize(config);

    // Run the library which will start the message loop
    wb::WindowsBaseLibrary::Run();

    // Shutdown the library
    wb::WindowsBaseLibrary::Shutdown();

    return 0;
}