#pragma once

#include <string>
#include <windows.h>
#include <functional>
#include <DirectXMath.h>

#include "bdc_ui/include/dll_config.h"
#include "riaecs/riaecs.h"
#include "bdc_ui/include/window_ui.h"

namespace bdc_ui
{

class BDC_UI_API ButtonUIDrawer : public WindowUIDrawer
{
public:
    // Alias for button click callback function
    using ButtonUIClickCallback = std::function<void()>;

    ButtonUIDrawer(
        HWND hwnd, float base_window_width, float base_window_height,
        std::string button_text, float font_size,
        DirectX::XMINT4 text_color, DirectX::XMINT4 hover_text_color,
        float pos_x, float pos_y,
        float btn_width, float btn_height,
        size_t non_hovering_tex_asset_id, size_t hovering_tex_asset_id,
        riaecs::IAssetContainer& asset_cont, std::string imgui_window_name,
        ButtonUIClickCallback click_callback);

    ButtonUIDrawer(
        HWND hwnd, float base_window_width, float base_window_height,
        std::string button_text, float font_size,
        DirectX::XMINT4 text_color, DirectX::XMINT4 hover_text_color,
        float pos_x, float pos_y,
        float btn_width, float btn_height,
        riaecs::IAssetContainer& asset_cont, std::string imgui_window_name,
        ButtonUIClickCallback click_callback);

    virtual ~ButtonUIDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

private:
    // Button click callback function
    ButtonUIClickCallback click_callback_ = nullptr;

    // Whether to use image for button
    bool use_image_ = true;

    // Non hovering button texture asset ID
    size_t non_hovering_btn_tex_asset_id_ = 0;

    // Hovering button texture asset ID
    size_t hovering_btn_tex_asset_id_ = 0;

    // Reference to asset container
    riaecs::IAssetContainer& asset_cont_;

    // ImGui window name
    std::string imgui_window_name_ = "";

    // Button text
    std::string button_text_ = "";

    // Text color
    DirectX::XMINT4 text_color_ = DirectX::XMINT4(255, 255, 255, 255);

    // Hover text color
    DirectX::XMINT4 hover_text_color_ = DirectX::XMINT4(255, 255, 0, 255);

    // Font size
    float font_size_ = 16.0f;

    // Draw position X
    float pos_x_ = 0.0f;

    // Draw position Y
    float pos_y_ = 0.0f;

    // Button width
    float btn_width_ = 0.0f;

    // Button height
    float btn_height_ = 0.0f;

};

} // namespace bdc_ui