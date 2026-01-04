#pragma once

#include "ecs/include/entity.h"
#include "asset_loader/include/asset_handle.h"
#include "mono_graphics_extension/include/ui_drawer.h"

#include "mono_entity_archive_extension/include/dll_config.h"
#include "mono_service/include/service.h"

namespace mono_entity_archive_extension
{

constexpr const float ASSET_BROWSER_WINDOW_WIDTH_RATIO = 0.3f;
constexpr const float ASSET_BROWSER_WINDOW_HEIGHT_RATIO = 0.3f;

constexpr const float ASSET_BROWSER_WINDOW_POS_X_RATIO = 0.5f - (ASSET_BROWSER_WINDOW_WIDTH_RATIO / 2.0f);
constexpr const float ASSET_BROWSER_WINDOW_POS_Y_RATIO = 0.5f - (ASSET_BROWSER_WINDOW_HEIGHT_RATIO / 2.0f);

class MONO_ENTITY_ARCHIVE_EXT_DLL AssetBrowserDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    AssetBrowserDrawer(
        std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy);
        
    ~AssetBrowserDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    // Check if there is a request to add a new asset
    bool HasAddAssetRequest() const { return request_add_asset_; }

    // Clear the add asset request flag
    void ClearAddAssetRequest() { request_add_asset_ = false; }

    // The structure for asset release request
    struct AssetReleaseRequest
    {
        bool requested = false;
        asset_loader::AssetHandleID asset_id = asset_loader::AssetHandleID();
    };

    // Check if there is a request to release an asset
    bool HasReleaseAssetRequest() const { return request_release_asset_.requested; }

    // Get the asset release request
    AssetReleaseRequest GetReleaseAssetRequest() const { return request_release_asset_; }

    // Clear the release asset request flag
    void ClearReleaseAssetRequest() { request_release_asset_ = AssetReleaseRequest(); }

private:
    // Asset service proxy
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy_ = nullptr;

    // Index of the asset being renamed
    size_t renaming_index_ = 0;

    // Whether renaming is in progress
    bool renaming_ = false;

    // Buffer for renaming input
    char rename_buffer_[256];

    // Flag to request focus on the input box
    bool request_focus_ = false;

    // Flag to request adding a new asset
    bool request_add_asset_ = false;

    // Asset release request
    AssetReleaseRequest request_release_asset_ = AssetReleaseRequest();
};

} // namespace mono_entity_archive_extension
