#include "mono_asset_service/src/pch.h"
#include "mono_asset_service/include/asset_service_command_list.h"

#include "utility_header/logger.h"

#include "mono_asset_service/src/json.hpp"
#include "mono_asset_service/include/export_config.h"
#include "mono_asset_service/include/asset_service.h"

namespace mono_asset_service
{

void AssetServiceCommandList::RegisterLoader(
    asset_loader::AssetLoaderID id, std::unique_ptr<asset_loader::AssetLoader> loader)
{
    AddCommand([id, loader = std::move(loader)](mono_service::ServiceAPI& service_api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Register asset loader
        asset_service_api.GetLoaderRegistrar().Register(id, std::move(loader));

        return true;
    });
}

void AssetServiceCommandList::UnregisterLoader(asset_loader::AssetLoaderID id)
{
    AddCommand([id](mono_service::ServiceAPI& service_api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Unregister asset loader
        asset_service_api.GetLoaderUnregistrar().Unregister(id);

        return true;
    });
}

void AssetServiceCommandList::LoadAsset(std::unique_ptr<asset_loader::AssetSource> asset_source)
{
    AddCommand([asset_source = std::move(asset_source)](mono_service::ServiceAPI& service_api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Store in asset sources map
        asset_service_api.GetLoadedAssetNameToHandleIDMap()[asset_source->source_data->GetName().data()]
            = asset_source->description->GetHandleID();

        // Add asset source
        // This will trigger the asset loading process in the asset service
        asset_service_api.GetAssetSources().emplace_back(std::move(asset_source));

        return true;
    });
}

void AssetServiceCommandList::ReleaseAsset(asset_loader::AssetHandleID handle_id)
{
    AddCommand([handle_id](mono_service::ServiceAPI& service_api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Get asset
        const asset_loader::Asset& asset = asset_service_api.GetAsset(handle_id);

        // Remove from loaded asset name to handle ID map
        assert(
            asset_service_api.GetLoadedAssetNameToHandleIDMap().find(asset.GetName().data())
            != asset_service_api.GetLoadedAssetNameToHandleIDMap().end() &&
            "Asset name not found in loaded asset name to handle ID map!");
        asset_service_api.GetLoadedAssetNameToHandleIDMap().erase(asset.GetName().data());

        // Unregister asset
        asset_service_api.GetAssetUnregistrar().UnregisterAsset(handle_id);

        return true;
    });
}

void AssetServiceCommandList::ReleaseLoadingAssetSource(asset_loader::AssetHandleID handle_id)
{
    AddCommand([handle_id](mono_service::ServiceAPI& service_api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Remove loading asset source
        auto& loading_asset_sources = asset_service_api.GetLoadingAssetSources();
        auto it = loading_asset_sources.find(handle_id);
        assert(it != loading_asset_sources.end() && "Loading asset source not found!");

        loading_asset_sources.erase(it);

        return true;
    });
}

void AssetServiceCommandList::ExportAssetDescriptionsToFile(
    const std::wstring& file_path, const std::vector<asset_loader::AssetHandleID>& asset_ids)
{
    AddCommand([file_path, asset_ids](mono_service::ServiceAPI& service_api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
            "AssetServiceAPI must be derived from ServiceAPI.");
        AssetServiceAPI& asset_service_api = dynamic_cast<AssetServiceAPI&>(service_api);

        // Prepare JSON data for export
        nlohmann::json json_data;
        json_data[EXPORT_TAG_ASSETS] = nlohmann::json::array();

        // Get loaded asset ids
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_api.GetLoadedAssetIDs();

        // Iterate through asset IDs to export
        for (const asset_loader::AssetHandleID& asset_id : asset_ids)
        {
            // Create asset JSON object
            nlohmann::json asset_json;

            // Get asset
            const asset_loader::Asset& asset = asset_service_api.GetAsset(asset_id);

            if (asset.GetFilePath().empty())
                continue; // Skip assets without file path

            // Add to asset JSON
            asset_json[EXPORT_TAG_ASSET_NAME] = asset.GetName().data();
            asset_json[EXPORT_TAG_ASSET_TYPE] = asset.GetTypeName().data();
            asset_json[EXPORT_TAG_ASSET_FILE_PATH] = asset.GetFilePath().data();

            // Add asset JSON to JSON data
            json_data[EXPORT_TAG_ASSETS].push_back(asset_json);
        }

        // Write JSON data to file
        std::ofstream output_file(file_path);
        if (!output_file.is_open())
        {
            utility_header::ConsoleLogErr(
                {"Failed to open file for writing"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        output_file << json_data.dump(4);
        if (!output_file)
        {
            utility_header::ConsoleLogErr(
                {"Failed to write JSON data to file"}, __FILE__, __LINE__, __FUNCTION__);
            return false; // Failure
        }

        // Close the file
        output_file.close();

        return true;
    });
}

} // namespace mono_asset_service