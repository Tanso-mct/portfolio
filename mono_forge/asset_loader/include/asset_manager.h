#pragma once

#include "utility_header/registry.h"
#include "class_template/singleton.h"

#include "asset_loader/include/dll_config.h"
#include "asset_loader/include/asset.h"
#include "asset_loader/include/asset_handle.h"

namespace asset_loader
{

// The registry for all assets
// It allows for registering and retrieving assets by their handle ID
using AssetRegistry = utility_header::Registry<Asset>;

// The asset manager class that manages assets and their handles
// It provides methods to retrieve assets by their ID
class ASSET_LOADER_DLL AssetManager :
    public class_template::Singleton<AssetManager>
{
public:
    AssetManager(AssetRegistry& registry) : 
        registry_(registry) 
    {
    }

    ~AssetManager() override = default;

    template <typename Func>
    void WithLock(Func&& func) 
    {
        registry_.WithUniqueLock([&](AssetRegistry& registry) 
        {
            func(*this);
        });
    }

    // Wrap the Get method of the registry
    // It must be called in WithLock lambda function
    Asset& GetAsset(AssetHandleID id) const;

    // Wrap the Contains method of the registry
    // It must be called in WithLock lambda function
    bool Contains(AssetHandleID id) const;

    // Wrap the GetCount method of the registry
    // It must be called in WithLock lambda function
    size_t GetAssetCount() const;

    // Wrap the GetRegisteredIDs method of the registry
    // It must be called in WithLock lambda function
    std::vector<AssetHandleID> GetRegisteredIDs() const;

private:
    // The asset registry reference
    AssetRegistry& registry_;
};

class ASSET_LOADER_DLL AssetRegistrar
{
public:
    AssetRegistrar(AssetRegistry& registry) : registry_(registry) {}
    ~AssetRegistrar() = default;

    // Register a new asset with the given ID
    void RegisterAsset(AssetHandleID id, std::unique_ptr<Asset> asset);

private:
    // The asset registry reference
    AssetRegistry& registry_;
};

class ASSET_LOADER_DLL AssetUnregistrar
{
public:
    AssetUnregistrar(AssetRegistry& registry) : registry_(registry) {}
    ~AssetUnregistrar() = default;

    // Unregister the asset associated with the given ID
    void UnregisterAsset(AssetHandleID id);

private:
    // The asset registry reference
    AssetRegistry& registry_;
};

} // namespace asset_loader