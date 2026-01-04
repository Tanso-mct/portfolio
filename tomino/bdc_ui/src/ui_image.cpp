#include "bdc_ui/src/pch.h"
#include "bdc_ui/include/ui_image.h"

#include "mono_asset/mono_asset.h"
#pragma comment(lib, "mono_asset.lib")

#include "mono_adapter/include/service_adapter.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace bdc_ui
{

ImageUIDrawer::ImageUIDrawer(
    HWND hwnd, float base_window_width, float base_window_height,
    float pos_x, float pos_y,
    float image_width, float image_height,
    size_t image_texture_asset_id, riaecs::IAssetContainer& asset_cont, std::string imgui_window_name) :
    WindowUIDrawer(hwnd, base_window_width, base_window_height),
    pos_x_(pos_x),
    pos_y_(pos_y),
    image_width_(image_width),
    image_height_(image_height),
    image_texture_asset_id_(image_texture_asset_id),
    asset_cont_(asset_cont),
    imgui_window_name_(imgui_window_name)
{
}

render_graph::ImguiPass::DrawFunc ImageUIDrawer::CreateDrawFunc()
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

        // Get image texture asset
        riaecs::ROObject<riaecs::IAsset> image_texture_asset
            = asset_cont_.Get({image_texture_asset_id_, asset_cont_.GetGeneration(image_texture_asset_id_)});
        const mono_asset::AssetTexture* image_texture_asset_ptr 
            = dynamic_cast<const mono_asset::AssetTexture*>(&image_texture_asset());
        assert(image_texture_asset_ptr != nullptr);

        // Get texture gpu descriptor handle
        D3D12_GPU_DESCRIPTOR_HANDLE texture_srv_handle
            = graphics_view->GetTexture2DSRVHandle(image_texture_asset_ptr->GetTextureHandle());
        ImTextureID imgui_texture_id = (ImTextureID)texture_srv_handle.ptr;

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

        // Consider the basic window size for image size ratio calculation
        float image_width_ratio = image_width_ / GetBaseWindowWidth();
        float image_height_ratio = image_height_ / GetBaseWindowHeight();

        // Get image size
        ImVec2 image_size = ImVec2(
            window_size.x * image_width_ratio, window_size.y * image_height_ratio);

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

        // Draw image
        ImGui::Image(imgui_texture_id, image_size);

        // End ImGui window
        ImGui::End();

        return true; // Success
    };
}

} // namespace bdc_ui