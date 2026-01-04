#pragma once

#include "mono_render/include/ui_drawer.h"

namespace bdc_ui
{

class WindowUIDrawer : public mono_render::UIDrawer
{
public:
    WindowUIDrawer(
        HWND hwnd, float base_window_width, float base_window_height) : 
        hwnd_(hwnd), base_window_width_(base_window_width), base_window_height_(base_window_height)
    {
        assert(hwnd_ != nullptr && "HWND must not be null");
    }
    virtual ~WindowUIDrawer() override = default;

protected:
    // Get handle to the application window
    HWND GetHwnd() const { return hwnd_; }

    // Get base window width
    float GetBaseWindowWidth() const { return base_window_width_; }

    // Get base window height
    float GetBaseWindowHeight() const { return base_window_height_; }
    
private:
    // Handle to the application window
    HWND hwnd_ = nullptr;

    // Base window width
    float base_window_width_ = 1920.0f;

    // Base window height
    float base_window_height_ = 1080.0f;
};

} // namespace bdc_ui