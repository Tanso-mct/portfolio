#include "asset_loader/src/pch.h"
#include "asset_loader/include/asset_manager.h"

namespace asset_loader
{

Asset& AssetManager::GetAsset(AssetHandleID id) const
{
    return registry_.Get(id);
}

bool AssetManager::Contains(AssetHandleID id) const
{
    return registry_.Contains(id);
}

size_t AssetManager::GetAssetCount() const
{
    return registry_.GetCount();
}

std::vector<AssetHandleID> AssetManager::GetRegisteredIDs() const
{
    return registry_.GetRegisteredIDs();
}

void AssetRegistrar::RegisterAsset(AssetHandleID id, std::unique_ptr<Asset> asset)
{
    registry_.WithUniqueLock([&](AssetRegistry& registry) 
    {
        registry.Register(id, std::move(asset));
    });
}

void AssetUnregistrar::UnregisterAsset(AssetHandleID id)
{
    registry_.WithUniqueLock([&](AssetRegistry& registry) 
    {
        registry.Unregister(id);
    });
}

} // namespace asset_loader