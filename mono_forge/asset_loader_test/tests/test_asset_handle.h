#pragma once

#include "asset_loader/include/asset_handle.h"

namespace asset_loader_test
{

struct TestAssetHandleTag {};
using TestAssetHandle = asset_loader::AssetHandle<TestAssetHandleTag>;


} // namespace asset_loader_test