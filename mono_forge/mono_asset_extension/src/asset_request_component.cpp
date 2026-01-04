#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/asset_request_component.h"

namespace mono_asset_extension
{

AssetRequestComponent::AssetRequestComponent()
{
}

AssetRequestComponent::~AssetRequestComponent()
{
}

bool AssetRequestComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    AssetRequestComponent::SetupParam* assetRequestComponent_param
        = dynamic_cast<AssetRequestComponent::SetupParam*>(&param);
    assert(assetRequestComponent_param != nullptr && "Invalid setup param type for AssetRequestComponent");

    // Set parameters
    needs_register_loaders_ = std::move(assetRequestComponent_param->needs_register_loaders);
    needs_load_asset_sources_ = std::move(assetRequestComponent_param->needs_load_asset_sources);

    return true; // Success
}

ecs::ComponentID AssetRequestComponent::GetID() const
{
    return AssetRequestComponentHandle::ID();
}

std::unique_ptr<asset_loader::AssetLoader> AssetRequestComponent::ExtractLoaderToRegister(
    asset_loader::AssetLoaderID id)
{
    // Find the loader by ID
    auto it = needs_register_loaders_.find(id);

    assert(it != needs_register_loaders_.end() && "Loader ID not found");
    assert(it->second != nullptr && "Loader is already extracted");

    // Extract the loader
    std::unique_ptr<asset_loader::AssetLoader> loader = std::move(it->second);

    return loader; // Return the extracted loader
}

void AssetRequestComponent::AddLoaderToRegister(std::unique_ptr<asset_loader::AssetLoader> loader)
{
    assert(loader != nullptr && "Cannot add null loader");

    // Get the loader ID
    asset_loader::AssetLoaderID id = loader->GetID();

    assert(
        needs_register_loaders_.find(id) == needs_register_loaders_.end() && 
        "Loader with the same ID already exists");

    // Insert the loader into the map
    needs_register_loaders_.emplace(id, std::move(loader));
}

std::unique_ptr<asset_loader::AssetSource> AssetRequestComponent::ExtractAssetSourceToLoad(size_t index)
{
    assert(index < needs_load_asset_sources_.size() && "Asset source index out of range");

    // Extract the asset source
    std::unique_ptr<asset_loader::AssetSource> asset_source = std::move(needs_load_asset_sources_[index]);

    return asset_source; // Return the extracted asset source
}

void AssetRequestComponent::AddAssetSourceToLoad(std::unique_ptr<asset_loader::AssetSource> asset_source)
{
    assert(asset_source != nullptr && "Cannot add null asset source");

    // Check there is no duplicate asset source by handle ID
    for (const auto& existing_asset_source : needs_load_asset_sources_)
    {
        assert(
            existing_asset_source->description->GetHandleID() != asset_source->description->GetHandleID() &&
            "Asset source with the same handle ID already exists");
    }

    // Add the asset source to the list
    needs_load_asset_sources_.emplace_back(std::move(asset_source));
}

void AssetRequestComponent::AddLoaderIDToUnregister(asset_loader::AssetLoaderID id)
{
    // Check there is no duplicate loader ID
    auto it = std::find(
        needs_unregister_loader_ids_.begin(), needs_unregister_loader_ids_.end(), id);
    assert(it == needs_unregister_loader_ids_.end() && "Loader ID already exists in the unregister list");

    // Add the loader ID to the list
    needs_unregister_loader_ids_.emplace_back(id);
}

void AssetRequestComponent::AddAssetHandleIDToRelease(asset_loader::AssetHandleID id)
{
    // Check there is no duplicate asset handle ID
    auto it = std::find(
        needs_release_asset_handle_ids_.begin(), needs_release_asset_handle_ids_.end(), id);
    assert(it == needs_release_asset_handle_ids_.end() && "Asset handle ID already exists in the release list");

    // Add the asset handle ID to the list
    needs_release_asset_handle_ids_.emplace_back(id);
}

} // namespace mono_asset_extension