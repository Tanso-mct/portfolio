#pragma once
#include "mono_d3d12/include/dll_config.h"

#include <Windows.h>
#include <unordered_map>
#include <vector>
#include <shared_mutex>

namespace mono_d3d12
{
    struct WindowMessage
    {
        UINT message;
        WPARAM wParam;
        LPARAM lParam;

        WindowMessage(UINT message, WPARAM wParam, LPARAM lParam)
            :  message(message), wParam(wParam), lParam(lParam) {}
    };

    class MONO_D3D12_API WindowMessageState
    {
    private:
        std::shared_mutex mutex_;
        std::unordered_map<HWND, std::vector<WindowMessage>> messages_;

        WindowMessageState() = default;
    public:
        static WindowMessageState& GetInstance();

        void AddMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
        std::vector<WindowMessage> TakeMessages(HWND hwnd);
    };
    
} // namespace mono_d3d12