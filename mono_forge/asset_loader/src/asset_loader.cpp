#include "asset_loader/src/pch.h"
#include "asset_loader/include/asset_loader.h"

namespace asset_loader
{

AssetLoader& AssetLoaderManager::GetLoader(AssetLoaderID id) const
{
    return registry_.Get(id);
}

bool AssetLoaderManager::Contains(AssetLoaderID id) const
{
    return registry_.Contains(id);
}

size_t AssetLoaderManager::GetLoaderCount() const
{
    return registry_.GetCount();
}

std::vector<AssetLoaderID> AssetLoaderManager::GetRegisteredLoaderIDs() const
{
    return registry_.GetRegisteredIDs();
}

void AssetLoaderRegistrar::Register(AssetLoaderID id, std::unique_ptr<AssetLoader> loader)
{
    registry_.WithUniqueLock([&](AssetLoaderRegistry& registry) 
    {
        if (registry.Contains(id))
            return;

        registry.Register(id, std::move(loader));
    });
}

void AssetLoaderUnregistrar::Unregister(AssetLoaderID id)
{
    registry_.WithUniqueLock([&](AssetLoaderRegistry& registry) 
    {
        registry.Unregister(id);
    });
}

} // namespace asset_loader