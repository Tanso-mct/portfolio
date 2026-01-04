#pragma once

#include <string>
#include <windows.h>

#include "bdc_ui/include/dll_config.h"
#include "riaecs/riaecs.h"
#include "bdc_ui/include/window_ui.h"

namespace bdc_ui
{

class BDC_UI_API ImageUIDrawer : public WindowUIDrawer
{
public:
    ImageUIDrawer(
        HWND hwnd, float base_window_width, float base_window_height,
        float pos_x, float pos_y,
        float image_width, float image_height,
        size_t image_texture_asset_id, riaecs::IAssetContainer& asset_cont, std::string imgui_window_name);
    virtual ~ImageUIDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

private:
    // Image texture asset ID
    size_t image_texture_asset_id_ = 0;

    // Reference to asset container
    riaecs::IAssetContainer& asset_cont_;

    // ImGui window name
    std::string imgui_window_name_ = "";

    // Draw position X
    float pos_x_ = 0.0f;

    // Draw position Y
    float pos_y_ = 0.0f;

    // Image width
    float image_width_ = 0.0f;

    // Image height
    float image_height_ = 0.0f;
};

} // namespace bdc_ui