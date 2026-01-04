#include "windows_base/src/pch.h"
#include "windows_base/include/asset_factory_registry.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"

void wb::AssetFactoryRegistry::AddFactory(size_t id, std::unique_ptr<IAssetFactory> factory)
{
    if (factories_.find(id) != factories_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Asset factory with ID ", std::to_string(id), " already exists."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Add the factory to the registry
    factories_[id] = std::move(factory);

    // Save the key
    keys_.push_back(id);

    // Update the maximum ID if necessary
    if (id > maxId)
    {
        maxId = id;
    }
}

wb::IAssetFactory &wb::AssetFactoryRegistry::GetFactory(size_t id)
{
    if (factories_.find(id) == factories_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Asset factory with ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *factories_[id];
}

size_t wb::AssetFactoryRegistry::GetMaxID() const
{
    return maxId;
}

const std::vector<size_t> &wb::AssetFactoryRegistry::GetKeys() const
{
    return keys_;
}

WINDOWS_BASE_API wb::AssetFactoryRegistry wb::gAssetFactoryRegistry;

wb::AssetFactoryRegistrar::AssetFactoryRegistrar(size_t id, std::unique_ptr<IAssetFactory> factory)
{
    wb::gAssetFactoryRegistry.AddFactory(id, std::move(factory));
}