#include "mono_asset_service/src/pch.h"
#include "mono_asset_service/include/asset_service_command_list.h"

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

} // namespace mono_asset_service