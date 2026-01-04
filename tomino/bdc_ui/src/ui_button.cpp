#include "bdc_ui/src/pch.h"
#include "bdc_ui/include/ui_button.h"

#include "mono_asset/mono_asset.h"
#pragma comment(lib, "mono_asset.lib")

#include "mono_adapter/include/service_adapter.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace bdc_ui
{

ButtonUIDrawer::ButtonUIDrawer(
    HWND hwnd, float base_window_width, float base_window_height,
    std::string button_text, float font_size,
    DirectX::XMINT4 text_color, DirectX::XMINT4 hover_text_color,
    float pos_x, float pos_y,
    float btn_width, float btn_height,
    size_t non_hovering_tex_asset_id, size_t hovering_tex_asset_id,
    riaecs::IAssetContainer& asset_cont, std::string imgui_window_name,
    ButtonUIClickCallback click_callback) :
    WindowUIDrawer(hwnd, base_window_width, base_window_height),
    button_text_(button_text),
    text_color_(text_color),
    hover_text_color_(hover_text_color),
    font_size_(font_size),
    pos_x_(pos_x),
    pos_y_(pos_y),
    btn_width_(btn_width),
    btn_height_(btn_height),
    non_hovering_btn_tex_asset_id_(non_hovering_tex_asset_id),
    hovering_btn_tex_asset_id_(hovering_tex_asset_id),
    asset_cont_(asset_cont),
    imgui_window_name_(imgui_window_name),
    click_callback_(click_callback),
    use_image_(true)
{
    assert(click_callback_ != nullptr && "ButtonUIClickCallback must not be null");
}

ButtonUIDrawer::ButtonUIDrawer(
    HWND hwnd, float base_window_width, float base_window_height,
    std::string button_text, float font_size,
    DirectX::XMINT4 text_color, DirectX::XMINT4 hover_text_color,
    float pos_x, float pos_y,
    float btn_width, float btn_height,
    riaecs::IAssetContainer& asset_cont, std::string imgui_window_name,
    ButtonUIClickCallback click_callback) :
    WindowUIDrawer(hwnd, base_window_width, base_window_height),
    button_text_(button_text),
    text_color_(text_color),
    hover_text_color_(hover_text_color),
    font_size_(font_size),
    pos_x_(pos_x),
    pos_y_(pos_y),
    btn_width_(btn_width),
    btn_height_(btn_height),
    asset_cont_(asset_cont),
    imgui_window_name_(imgui_window_name),
    click_callback_(click_callback),
    use_image_(false)
{
    assert(click_callback_ != nullptr && "ButtonUIClickCallback must not be null");
}

render_graph::ImguiPass::DrawFunc ButtonUIDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        // Get singleton graphics service adapter
        mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
            = mono_adapter::GraphicsServiceAdapter::GetInstance();

        // Create graphics service proxy
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy 
            = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();

        // Create graphics service view
        std::unique_ptr<mono_service::ServiceView> graphics_service_view = graphics_service_proxy->CreateView();
        mono_graphics_service::GraphicsServiceView* graphics_view
            = dynamic_cast<mono_graphics_service::GraphicsServiceView*>(graphics_service_view.get());
        assert(graphics_view != nullptr);

        // Get non-hovering button texture asset
        riaecs::ROObject<riaecs::IAsset> non_hovering_texture_asset
            = asset_cont_.Get({non_hovering_btn_tex_asset_id_, asset_cont_.GetGeneration(non_hovering_btn_tex_asset_id_)});
        const mono_asset::AssetTexture* non_hovering_texture_asset_ptr 
            = dynamic_cast<const mono_asset::AssetTexture*>(&non_hovering_texture_asset());
        assert(non_hovering_texture_asset_ptr != nullptr);

        // Get hovering button texture asset
        riaecs::ROObject<riaecs::IAsset> hovering_texture_asset
            = asset_cont_.Get({hovering_btn_tex_asset_id_, asset_cont_.GetGeneration(hovering_btn_tex_asset_id_)});
        const mono_asset::AssetTexture* hovering_texture_asset_ptr 
            = dynamic_cast<const mono_asset::AssetTexture*>(&hovering_texture_asset());
        assert(hovering_texture_asset_ptr != nullptr);

        // Get non hovering texture gpu descriptor handle
        D3D12_GPU_DESCRIPTOR_HANDLE non_hovering_texture_srv_handle
            = graphics_view->GetTexture2DSRVHandle(non_hovering_texture_asset_ptr->GetTextureHandle());
        ImTextureID non_hovering_imgui_texture_id = (ImTextureID)non_hovering_texture_srv_handle.ptr;

        // Get hovering texture gpu descriptor handle
        D3D12_GPU_DESCRIPTOR_HANDLE hovering_texture_srv_handle
            = graphics_view->GetTexture2DSRVHandle(hovering_texture_asset_ptr->GetTextureHandle());
        ImTextureID hovering_imgui_texture_id = (ImTextureID)hovering_texture_srv_handle.ptr;

        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        // Get window size
        RECT client_rect;
        GetClientRect(GetHwnd(), &client_rect);
        ImVec2 window_size = ImVec2(
            static_cast<float>(client_rect.right - client_rect.left),
            static_cast<float>(client_rect.bottom - client_rect.top));

        if (window_size.x <= 0.0f || window_size.y <= 0.0f)
            return true; // Success, but nothing to draw

        // Consider the basic window size for ratio calculation
        float btn_width_ratio = btn_width_ / GetBaseWindowWidth();
        float btn_height_ratio = btn_height_ / GetBaseWindowHeight();

        // Get button size
        ImVec2 image_size = ImVec2(
            window_size.x * btn_width_ratio, window_size.y * btn_height_ratio);

        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));
        ImGui::SetNextWindowSize(window_size);

        // Begin ImGui window
        ImGui::Begin(
            imgui_window_name_.c_str(), nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBackground);

        // Consider the basic window size for position calculation
        float pos_x_ratio = pos_x_ / GetBaseWindowWidth();
        float pos_y_ratio = pos_y_ / GetBaseWindowHeight();

        // Create drawing position
        ImVec2 draw_pos = ImVec2(
            window_size.x * pos_x_ratio - (image_size.x * 0.5f),
            window_size.y * pos_y_ratio - (image_size.y * 0.5f));

        // Set cursor position
        ImGui::SetCursorPos(draw_pos);

        // Create button name
        if (ImGui::InvisibleButton(imgui_window_name_.c_str(), image_size))
        {
            // Button was clicked, Handle click callback
            click_callback_();
        }

        // Get item rect
        ImVec2 min = ImGui::GetItemRectMin();
        ImVec2 max = ImGui::GetItemRectMax();

        // Set texture based on hovering state
        bool hovered = ImGui::IsItemHovered();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (hovered)
        {
            if (use_image_)
            {
                // Draw hovering button image
                dl->AddImage(hovering_imgui_texture_id, min, max);
            }

            // Create hover text color
            ImU32 hover_text_col = IM_COL32(
                static_cast<UINT8>(hover_text_color_.x),
                static_cast<UINT8>(hover_text_color_.y),
                static_cast<UINT8>(hover_text_color_.z),
                static_cast<UINT8>(hover_text_color_.w));

            // Draw button text with hover color
            dl->AddText(NULL, font_size_, draw_pos, hover_text_col, button_text_.c_str());
        }
        else
        {
            if (use_image_)
            {
                // Draw non-hovering button image
                dl->AddImage(non_hovering_imgui_texture_id, min, max);
            }

            // Create normal text color
            ImU32 text_col = IM_COL32(
                static_cast<UINT8>(text_color_.x),
                static_cast<UINT8>(text_color_.y),
                static_cast<UINT8>(text_color_.z),
                static_cast<UINT8>(text_color_.w));

            // Draw button text with normal color
            dl->AddText(NULL, font_size_, draw_pos, text_col, button_text_.c_str());
        }
        
        // End ImGui window
        ImGui::End();

        return true; // Success
    };
}

} // namespace bdc_ui