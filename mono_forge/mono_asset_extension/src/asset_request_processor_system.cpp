#include "mono_asset_extension/src/pch.h"
#include "mono_asset_extension/include/asset_request_processor_system.h"

#include "ecs/include/world.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/asset_request_component.h"

namespace mono_asset_extension
{

bool RegisterLoaders(
    AssetRequestComponent* request_component, mono_asset_service::AssetServiceCommandList* asset_command_list)
{
    for (const auto& [loader_id, loader] : request_component->GetLoadersToRegister())
            asset_command_list->RegisterLoader(loader_id, request_component->ExtractLoaderToRegister(loader_id));

    // Clear needs to register loaders after extraction
    request_component->ClearLoadersToRegister();

    return true; // Success
}

bool LoadAssets(
    AssetRequestComponent* request_component, mono_asset_service::AssetServiceCommandList* asset_command_list)
{
    // Load asset sources
    for (size_t i = 0; i < request_component->GetAssetSourcesToLoad().size(); ++i)
        asset_command_list->LoadAsset(request_component->ExtractAssetSourceToLoad(i));

    // Clear needs to load asset sources after extraction
    request_component->ClearAssetSourcesToLoad();

    return true; // Success
}

bool UnregisterLoaders(
    AssetRequestComponent* request_component, mono_asset_service::AssetServiceCommandList* asset_command_list)
{
    // Unregister loader IDs
    for (const auto& loader_id : request_component->GetLoaderIDsToUnregister())
        asset_command_list->UnregisterLoader(loader_id);

    // Clear needs to unregister loader IDs after extraction
    request_component->ClearLoaderIDsToUnregister();

    return true; // Success
}

bool ReleaseAssets(
    AssetRequestComponent* request_component, mono_asset_service::AssetServiceCommandList* asset_command_list)
{
    // Release asset handle IDs
    for (const auto& handle_id : request_component->GetAssetHandleIDsToRelease())
        asset_command_list->ReleaseAsset(handle_id);

    // Clear needs to release asset handle IDs after extraction
    request_component->ClearAssetHandleIDsToRelease();

    return true; // Success
}

AssetRequestProcessorSystem::AssetRequestProcessorSystem(
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy) :
    asset_service_proxy(std::move(asset_service_proxy))
{
}

AssetRequestProcessorSystem::~AssetRequestProcessorSystem()
{
}

bool AssetRequestProcessorSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool AssetRequestProcessorSystem::Update(ecs::World& world)
{
    for (const ecs::Entity& entity : world.View(AssetRequestComponentHandle::ID())())
    {
        AssetRequestComponent* request_component 
            = world.GetComponent<AssetRequestComponent>(entity, AssetRequestComponentHandle::ID());
        assert(request_component != nullptr && "AssetRequestComponent should exist on the entity.");

        // Create asset service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list 
            = asset_service_proxy->CreateCommandList();
        mono_asset_service::AssetServiceCommandList* asset_command_list
            = dynamic_cast<mono_asset_service::AssetServiceCommandList*>(command_list.get());
        assert(asset_command_list != nullptr && "Failed to create AssetServiceCommandList.");

        // Register loaders
        if (!RegisterLoaders(request_component, asset_command_list))
            return false; // Failure

        // Load assets
        if (!LoadAssets(request_component, asset_command_list))
            return false; // Failure

        // Unregister loaders
        if (!UnregisterLoaders(request_component, asset_command_list))
            return false; // Failure

        // Release assets
        if (!ReleaseAssets(request_component, asset_command_list))
            return false; // Failure

        // Submit the command list
        asset_service_proxy->SubmitCommandList(std::move(command_list));
    }

    return true; // Success
}

bool AssetRequestProcessorSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID AssetRequestProcessorSystem::GetID() const
{
    return AssetRequestProcessorSystemHandle::ID();
}

} // namespace mono_asset_extension