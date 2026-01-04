#include "asset_loader/src/pch.h"
#include "asset_loader/include/asset_description.h"

namespace asset_loader
{

asset_loader::AssetDescription::AssetDescription(
    AssetHandleID handle_id, AssetLoaderID loader_id) :
    handle_id_(handle_id),
    loader_id_(loader_id)
{
}


} // namespace asset_loader