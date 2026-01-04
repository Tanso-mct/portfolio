#pragma once

#include "class_template/non_copy.h"

#include "asset_loader/include/dll_config.h"
#include "asset_loader/include/asset_description.h"
#include "asset_loader/include/asset_loader.h"

namespace asset_loader 
{

// Struct that holds asset source data, staging area, and description
class AssetSource :
    public class_template::NonCopyable
{
public:
    std::unique_ptr<asset_loader::AssetSourceData> source_data;
    std::unique_ptr<asset_loader::AssetDescription> description;
};

// Type alias for a vector of unique pointers to AssetSource
using AssetSources = std::vector<std::unique_ptr<AssetSource>>;

} // namespace asset_loader

