#pragma once

#include "utility_header/id.h"
#include "class_template/singleton.h"

#include "asset_loader/include/dll_config.h"

namespace asset_loader
{

// The type used to identify asset handles
using AssetHandleID = utility_header::ID;

// The ID generator for asset handles
class ASSET_LOADER_DLL AssetHandleIDGenerator :
    public class_template::Singleton<AssetHandleIDGenerator>,
    public utility_header::IDGenerator
{
public:
    AssetHandleIDGenerator() = default;
    virtual ~AssetHandleIDGenerator() override = default;
};

// The template class for asset handles of specific types
template <typename HandleTag>
class AssetHandle
{
public:
    AssetHandle() = default;
    virtual ~AssetHandle() = default;

    // Get the unique ID of the asset handle type
    static AssetHandleID ID()
    {
        static const AssetHandleID id = AssetHandleIDGenerator::GetInstance().Generate();
        return id;
    }
};

} // namespace asset_loader