#pragma once

#include "utility_header/locked_value.h"
#include "mono_service/include/service_view.h"

#include "mono_asset_service/include/dll_config.h"
#include "mono_asset_service/include/asset_service.h"

namespace mono_asset_service
{

class MONO_ASSET_SERVICE_DLL AssetServiceView :
    public mono_service::ServiceView
{
public:
    AssetServiceView(const mono_service::ServiceAPI& service_api);
    virtual ~AssetServiceView() override = default;

    // Get the asset for the given handle ID
    const asset_loader::Asset& GetAsset(asset_loader::AssetHandleID id) const;

    // Check if the asset for the given handle ID is loaded
    bool IsAssetLoaded(asset_loader::AssetHandleID id) const;

    // Get the list of loaded asset handle IDs
    std::vector<asset_loader::AssetHandleID> GetLoadedAssetIDs() const;

    // Get the asset handle ID by asset name
    asset_loader::AssetHandleID GetAssetHandleIDByName(std::string_view asset_name) const;
};

} // namespace mono_asset_service