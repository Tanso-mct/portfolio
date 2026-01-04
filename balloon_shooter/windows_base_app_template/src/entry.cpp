#include "$project_name$/src/pch.h"
#pragma comment(lib, "windows_base.lib")

#include "$project_name$/include/window/$window_name$_window.h"
#include "$project_name$/include/scene/$scene_name$_scene.h"

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
    config.createWindowIDs_ = { $project_name$::$WindowName$WindowID() };
    config.createSceneIDs_ = { $project_name$::$SceneName$SceneFacadeID() };

    // Initialize the Windows Base Library
    wb::WindowsBaseLibrary::Initialize(config);

    // Run the library which will start the message loop
    wb::WindowsBaseLibrary::Run();

    // Shutdown the library
    wb::WindowsBaseLibrary::Shutdown();

    return 0;
}