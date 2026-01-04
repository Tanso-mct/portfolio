#include "windows_base/src/pch.h"
#include "windows_base/include/asset_group.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

const std::vector<size_t> &wb::AssetGroup::GetAssetIDs() const
{
    return assetIDs_;
}

void wb::AssetGroup::AddAssetID(size_t assetID)
{
    assetIDs_.push_back(assetID);
}