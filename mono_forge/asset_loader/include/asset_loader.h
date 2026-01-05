#pragma once

#include "class_template/singleton.h"
#include "utility_header/id.h"
#include "utility_header/registry.h"

#include "asset_loader/include/dll_config.h"
#include "asset_loader/include/asset.h"

namespace asset_loader
{

// The type used to identify asset loaders
using AssetLoaderID = utility_header::ID;

// The ID generator for AssetLoaderID
class ASSET_LOADER_DLL AssetLoaderIDGenerator :
    public class_template::Singleton<AssetLoaderIDGenerator>,
    public utility_header::IDGenerator
{
public:
    AssetLoaderIDGenerator() = default;
    virtual ~AssetLoaderIDGenerator() override = default;
};

// The base class for staging area used during asset loading
// It allows for temporary storage and manipulation of data before the asset is finalized
class LoadingStagingArea
{
public:
    virtual ~LoadingStagingArea() = default;
};

// The data associated with an asset source
// You can inherit this class to create your own asset source data
class AssetSourceData
{
public:
    virtual ~AssetSourceData() = default;

    // Get the name of the asset
    std::string_view GetName() const { return name_; }

    // Set the name of the asset
    void SetName(const std::string& name) { name_ = name; }

private:
    std::string name_ = "";
};

// The interface for all asset loaders
// An asset loader is responsible for loading assets of a specific type
class AssetLoader
{
public:
    virtual ~AssetLoader() = default;

    // Get the unique ID of the asset loader type
    virtual AssetLoaderID GetID() const = 0;

    // Prepare a staging area for loading
    virtual std::unique_ptr<LoadingStagingArea> Prepare() const = 0;

    // Load an asset with source data and a staging area
    virtual std::unique_ptr<Asset> Load(AssetSourceData& source_data, LoadingStagingArea& staging_area) const = 0;

    // Commit any changes made during loading
    virtual bool Commit(LoadingStagingArea& staging_area) const = 0;
};

// The base class for all asset loaders
// It provides get the asset loader ID functionality
template <typename Derived>
class AssetLoaderBase :
    public AssetLoader
{
public:
    AssetLoaderBase() = default;
    virtual ~AssetLoaderBase() override = default;

    // Get the unique ID of the asset loader type
    static AssetLoaderID ID()
    {
        static const AssetLoaderID id = AssetLoaderIDGenerator::GetInstance().Generate();
        return id;
    }

    // Override the GetID method to return the unique ID of the asset loader type
    virtual AssetLoaderID GetID() const override
    {
        return ID();
    }
};

// The registry for all asset loaders
using AssetLoaderRegistry = utility_header::Registry<AssetLoader>;

class ASSET_LOADER_DLL AssetLoaderManager :
    public class_template::Singleton<AssetLoaderManager>
{
public:
    AssetLoaderManager(AssetLoaderRegistry& registry) : 
    registry_(registry) 
    {
    }

    virtual ~AssetLoaderManager() override = default;

    template <typename Func>
    void WithLock(Func&& func) 
    {
        registry_.WithUniqueLock([&](AssetLoaderRegistry& registry) 
        {
            func(*this);
        });
    }

    // Wrap the Get method of the registry
    // It must be called in WithLock lambda function
    AssetLoader& GetLoader(AssetLoaderID id) const;

    // Wrap the Contains method of the registry
    // It must be called in WithLock lambda function
    bool Contains(AssetLoaderID id) const;

    // Wrap the GetCount method of the registry
    // It must be called in WithLock lambda function
    size_t GetLoaderCount() const;

    // Wrap the GetRegisteredIDs method of the registry
    // It must be called in WithLock lambda function
    std::vector<AssetLoaderID> GetRegisteredLoaderIDs() const;

private:
    // The asset loader registry
    AssetLoaderRegistry& registry_;
};

class ASSET_LOADER_DLL AssetLoaderRegistrar
{
public:
    AssetLoaderRegistrar(AssetLoaderRegistry& registry) : registry_(registry) {}
    ~AssetLoaderRegistrar() = default;

    // Register a new asset loader with the given ID
    void Register(AssetLoaderID id, std::unique_ptr<AssetLoader> loader);

private:
    // The asset loader registry reference
    AssetLoaderRegistry& registry_;
};

class ASSET_LOADER_DLL AssetLoaderUnregistrar
{
public:
    AssetLoaderUnregistrar(AssetLoaderRegistry& registry) : registry_(registry) {}
    ~AssetLoaderUnregistrar() = default;

    // Unregister the asset loader associated with the given ID
    void Unregister(AssetLoaderID id);

private:
    // The asset loader registry reference
    AssetLoaderRegistry& registry_;
};

} // namespace asset_loader