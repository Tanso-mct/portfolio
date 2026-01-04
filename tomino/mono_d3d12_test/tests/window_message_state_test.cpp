#include "mono_d3d12_test/pch.h"

#include "mono_d3d12/include/window_message_state.h"
#pragma comment(lib, "mono_d3d12.lib")

#include <Windows.h>
#include <future>

static LRESULT CALLBACK WindowProcD3D12Helpers(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    mono_d3d12::WindowMessageState& messageState = mono_d3d12::WindowMessageState::GetInstance();
    messageState.AddMessage(hWnd, msg, wParam, lParam);

    switch (msg)
    {
    case WM_PAINT:
        break;

    default:
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

namespace
{
    void CreateWindowTemp(HWND& hWnd, WNDCLASSEX& wc, std::wstring className)
    {
        wc.lpszClassName = className.c_str();
        wc.hInstance = GetModuleHandle(nullptr);
        wc.lpfnWndProc = WindowProcD3D12Helpers;

        if (!RegisterClassEx(&wc))
        {
            FAIL() << "Failed to register window class: ";
        }

        hWnd = CreateWindow
        (
            wc.lpszClassName,
            className.c_str(),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT,
            800, 600,
            nullptr, nullptr, wc.hInstance, nullptr
        );
        if (!hWnd)
        {
            FAIL() << "Failed to create window";
        }

        ShowWindow(hWnd, SW_SHOW);
        UpdateWindow(hWnd);
    }
}

TEST(WindowMessageState, AddAndTakeMessages)
{
    HWND hWnd = nullptr;
    WNDCLASSEX wc = { sizeof(WNDCLASSEX) };
    wc.lpszClassName = L"WindowMessageStateTest";
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpfnWndProc = WindowProcD3D12Helpers;

    CreateWindowTemp(hWnd, wc, L"WindowMessageStateTest");

    bool isRunning = true;
    std::future<void> mainLoop = std::async
    (
        std::launch::async, [hWnd, &isRunning]()
        {
            while (isRunning)
            {
                mono_d3d12::WindowMessageState& messageState = mono_d3d12::WindowMessageState::GetInstance();
                std::vector<mono_d3d12::WindowMessage> wndMsgs = messageState.TakeMessages(hWnd);
                for (const auto& wndMsg : wndMsgs)
                {
                    if (wndMsg.message != WM_PAINT)
                        std::cout << "Message: " << wndMsg.message << std::endl;
                    
                    if (wndMsg.message == WM_DESTROY)
                        isRunning = false;
                }
            }
        }
    );

    // Run the message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (!isRunning)
            PostQuitMessage(0);
    }

    mainLoop.get();
    
    DestroyWindow(hWnd);
}