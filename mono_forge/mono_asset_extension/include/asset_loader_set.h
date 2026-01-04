#pragma once

#include "mono_asset_extension/include/dll_config.h"

#include "mono_service/include/service_proxy_manager.h"
#include "mono_asset_extension/include/asset_request_component.h"

#include "mono_asset_extension/include/mesh_asset.h"
#include "mono_asset_extension/include/texture_asset.h"

namespace mono_asset_extension
{

class MONO_ASSET_EXT_DLL AssetLoaderSet
{
public:
    AssetLoaderSet(mono_service::ServiceProxyManager& service_proxy_manager) :
        service_proxy_manager_(service_proxy_manager)
    {
    }
    virtual ~AssetLoaderSet() = default;

    // Store loaders to the given loader map
    void StoreToMap(AssetRequestComponent::LoaderMap& loader_map);

    // Extract asset source to load by file path
    std::unique_ptr<asset_loader::AssetSource> ExtractAssetSourceToLoad(std::wstring_view file_path);

private:
    // The reference to the service proxy manager
    mono_service::ServiceProxyManager& service_proxy_manager_;
};

} // namespace mono_asset_extension