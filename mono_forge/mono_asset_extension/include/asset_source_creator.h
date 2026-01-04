#pragma once

#include <memory>
#include <string_view>

#include "asset_loader/include/asset_source.h"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_asset_extension
{

// The abstract base class for asset source creators
class AssetSourceCreator
{
public:
    virtual ~AssetSourceCreator() = default;

    // Create an asset source for the given file path
    virtual std::unique_ptr<asset_loader::AssetSource> CreateAssetSource(
        std::wstring_view file_path, mono_service::ServiceProxyManager& service_proxy_manager) = 0;
};

} // namespace mono_asset_extension