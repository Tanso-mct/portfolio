#pragma once

#include "mono_service/include/service_command.h"

#include "mono_asset_service/include/dll_config.h"
#include "mono_asset_service/include/asset_service.h"

namespace mono_asset_service
{

// The graphics command list class
class MONO_ASSET_SERVICE_DLL AssetServiceCommandList :
    public mono_service::ServiceCommandList
{
public:
    AssetServiceCommandList() = default;
    ~AssetServiceCommandList() override = default;

    // Register an asset loader
    void RegisterLoader(asset_loader::AssetLoaderID id, std::unique_ptr<asset_loader::AssetLoader> loader);

    // Unregister an asset loader
    void UnregisterLoader(asset_loader::AssetLoaderID id);

    // Load an asset the given asset source
    void LoadAsset(std::unique_ptr<asset_loader::AssetSource> asset_source);

    // Release an asset with the given handle ID
    void ReleaseAsset(asset_loader::AssetHandleID handle_id);

    // Release a loading asset source with the given handle ID
    void ReleaseLoadingAssetSource(asset_loader::AssetHandleID handle_id);

    // Export asset descriptions to a file
    void ExportAssetDescriptionsToFile(const std::wstring& file_path, const std::vector<asset_loader::AssetHandleID>& asset_ids);
};

} // namespace mono_asset_service