#pragma once

#include <memory>

#include "asset_loader/include/dll_config.h"
#include "asset_loader/include/asset_loader.h"
#include "asset_loader/include/asset_handle.h"

namespace asset_loader
{

// The description of an asset to be loaded
// It contains the asset loader ID and the source data required for loading
class ASSET_LOADER_DLL AssetDescription
{
public:
    AssetDescription(
        AssetHandleID handle_id, AssetLoaderID loader_id);

    virtual ~AssetDescription() = default;

    // Get the asset handle ID
    AssetHandleID GetHandleID() const
    {
        return handle_id_;
    }

    // Get the asset loader ID
    AssetLoaderID GetLoaderID() const
    {
        return loader_id_;
    }

private:
    const AssetHandleID handle_id_;
    const AssetLoaderID loader_id_;
};

} // namespace asset_loader