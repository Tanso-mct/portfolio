#include "mono_d3d12/src/pch.h"
#include "mono_d3d12/include/window_message_state.h"

mono_d3d12::WindowMessageState& mono_d3d12::WindowMessageState::GetInstance()
{
    static WindowMessageState instance;
    return instance;
}

void mono_d3d12::WindowMessageState::AddMessage(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    messages_[hwnd].emplace_back(message, wParam, lParam);
}

std::vector<mono_d3d12::WindowMessage> mono_d3d12::WindowMessageState::TakeMessages(HWND hwnd)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    
    auto it = messages_.find(hwnd);
    if (it != messages_.end())
    {
        std::vector<WindowMessage> messages = std::move(it->second);
        messages_.erase(it);
        return messages;
    }
    return {};
}