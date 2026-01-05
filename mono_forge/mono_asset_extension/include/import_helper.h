#pragma once

#include "mono_asset_extension/include/dll_config.h"
#include "mono_asset_service/src/json.hpp"
#include "asset_loader/include/asset_source.h"
#include "mono_service/include/service_proxy_manager.h"
#include "mono_asset_extension/include/asset_request_component.h"

namespace mono_asset_extension
{

// Create asset sources from exported JSON data
MONO_ASSET_EXT_DLL bool CreateAssetSourcesFromExportedJSON(
    const nlohmann::json& json, mono_service::ServiceProxyManager& service_proxy_manager,
    AssetRequestComponent::AssetSourceList& out_asset_sources);

} // namespace mono_asset_extension