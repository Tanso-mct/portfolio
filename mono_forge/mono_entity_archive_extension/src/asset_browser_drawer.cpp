#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/asset_browser_drawer.h"

#include "mono_asset_service/include/asset_service_view.h"

namespace mono_entity_archive_extension
{

AssetBrowserDrawer::AssetBrowserDrawer(
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy) :
    asset_service_proxy_(std::move(asset_service_proxy))
{
    assert(asset_service_proxy_ != nullptr && "Asset service proxy is null!");
}

render_graph::ImguiPass::DrawFunc AssetBrowserDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        // Begin the asset browser window
        if (!show_window_)
            return true;

        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        // Get IO object
        ImGuiIO& io = ImGui::GetIO();

        // Get window size
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;

        // Set new window position
        ImVec2 winPos = ImVec2(window_width * ASSET_BROWSER_WINDOW_POS_X_RATIO, window_height * ASSET_BROWSER_WINDOW_POS_Y_RATIO);
        ImGui::SetNextWindowPos(winPos, ImGuiCond_Once);

        // Set new window size
        ImVec2 winSize = ImVec2(window_width * ASSET_BROWSER_WINDOW_WIDTH_RATIO, window_height * ASSET_BROWSER_WINDOW_HEIGHT_RATIO);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Once);

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> service_view = asset_service_proxy_->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view
            = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());

        ImGui::Begin("AssetBrowser", &show_window_);

        // Get loaded asset IDs
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_view->GetLoadedAssetIDs();

        // Display each loaded asset with asset's name
        for (size_t i = 0; i < loaded_asset_ids.size(); ++i)
        {
            // Get asset ID
            asset_loader::AssetHandleID asset_id = loaded_asset_ids[i];

            // Push ID for ImGui
            ImGui::PushID(i);

            // Get asset
            const asset_loader::Asset& asset = asset_service_view->GetAsset(asset_id);

            // Create display text
            std::string display_text = std::string(asset.GetName()) + " (" + std::string(asset.GetTypeName()) + ")";

            // Display asset name
            ImGui::Selectable(display_text.c_str());

            if (renaming_index_ == i && renaming_)
            {
                if (request_focus_)
                {
                    ImGui::SetKeyboardFocusHere();
                    request_focus_ = false;
                }

                // Move to same line
                ImGui::SameLine();

                // Input text for renaming
                if (ImGui::InputText(
                    "##edit", rename_buffer_, IM_ARRAYSIZE(rename_buffer_), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    asset.SetName(rename_buffer_);
                    renaming_ = false;
                }

                // Check if input box is deactivated (focus lost)
                if (ImGui::IsItemDeactivated())
                {
                    asset.SetName(rename_buffer_);
                    renaming_ = false;
                }
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Rename"))
                {
                    renaming_index_ = i;
                    renaming_ = true;
                    request_focus_ = true;
                    strcpy(rename_buffer_, asset.GetName().data());
                }

                if (ImGui::MenuItem("Delete"))
                {
                    request_release_asset_.requested = true;
                    request_release_asset_.asset_id = asset_id;
                }

                ImGui::EndPopup();
            }

            // Pop ID for ImGui
            ImGui::PopID();
        }

        if (
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::SetNextWindowPos(io.MouseClickedPos[ImGuiMouseButton_Right]);
            ImGui::OpenPopup("AssetBrowserContextPopup");
        }

        if (ImGui::BeginPopup("AssetBrowserContextPopup"))
        {
            if (ImGui::MenuItem("Add Asset"))
                request_add_asset_ = true;

            ImGui::EndMenu();
        }

        // End the asset browser window
        ImGui::End();

        return true; // Success
    };
}

} // namespace mono_entity_archive_extension