#include "mono_service/src/pch.h"
#include "mono_asset_service/include/asset_service_view.h"

namespace mono_asset_service
{

AssetServiceView::AssetServiceView(const mono_service::ServiceAPI &service_api) :
    mono_service::ServiceView(service_api)
{
}

const asset_loader::Asset& AssetServiceView::GetAsset(asset_loader::AssetHandleID id) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
        "AssetServiceAPI must be derived from ServiceAPI.");
    const AssetServiceAPI& asset_service_api = dynamic_cast<const AssetServiceAPI&>(service_api_);

    // Return the asset
    return asset_service_api.GetAsset(id);
}

bool AssetServiceView::IsAssetLoaded(asset_loader::AssetHandleID id) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
        "AssetServiceAPI must be derived from ServiceAPI.");
    const AssetServiceAPI& asset_service_api = dynamic_cast<const AssetServiceAPI&>(service_api_);

    // Return whether the asset is loaded
    return asset_service_api.IsAssetLoaded(id);
}

std::vector<asset_loader::AssetHandleID> AssetServiceView::GetLoadedAssetIDs() const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, AssetServiceAPI>::value,
        "AssetServiceAPI must be derived from ServiceAPI.");
    const AssetServiceAPI& asset_service_api = dynamic_cast<const AssetServiceAPI&>(service_api_);

    // Return the list of loaded asset handle IDs
    return asset_service_api.GetLoadedAssetIDs();
}

} // namespace mono_asset_service