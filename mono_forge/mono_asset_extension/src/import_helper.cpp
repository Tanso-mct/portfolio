#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/import_helper.h"

#include "utility_header/win32.h"

#include "mono_asset_service/include/export_config.h"
#include "mono_asset_extension/include/asset_loader_set.h"

namespace mono_asset_extension
{

MONO_ASSET_EXT_DLL bool CreateAssetSourcesFromExportedJSON(
    const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager,
    AssetRequestComponent::AssetSourceList& out_asset_sources)
{
    // Get assets JSON
    const nlohmann::json& assets_json = json[mono_asset_service::EXPORT_TAG_ASSETS];

    // Create asset loader set
    mono_asset_extension::AssetLoaderSet asset_loader_set(service_proxy_manager);

    // Iterate through each asset JSON
    for (const auto& asset_json : assets_json)
    {
        // Get file path
        std::string file_path_str = asset_json[mono_asset_service::EXPORT_TAG_ASSET_FILE_PATH].get<std::string>();

        // Convert to wstring_view
        std::wstring file_path_wstr = utility_header::StringToWstring(file_path_str);

        // Extract asset source to load
        std::unique_ptr<asset_loader::AssetSource> asset_source
            = asset_loader_set.ExtractAssetSourceToLoad(file_path_wstr);

        // Get asset name
        std::string asset_name = asset_json[mono_asset_service::EXPORT_TAG_ASSET_NAME].get<std::string>();

        // Set asset name to source data
        asset_source->source_data->SetName(asset_name);

        // Add to output asset sources vector
        out_asset_sources.emplace_back(std::move(asset_source));
    }

    return true; // Success
}
    

} // namespace mono_asset_extension