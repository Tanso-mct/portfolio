#pragma once

#include <unordered_map>

#include "ecs/include/component.h"
#include "asset_loader/include/asset_loader.h"
#include "asset_loader/include/asset_source.h"
#include "mono_asset_extension/include/dll_config.h"

namespace mono_asset_extension
{

// The handle class for the component
class MONO_ASSET_EXT_DLL AssetRequestComponentHandle : public ecs::ComponentHandle<AssetRequestComponentHandle> {};

// The component class
class MONO_ASSET_EXT_DLL AssetRequestComponent :
    public ecs::Component
{
public:
    AssetRequestComponent();
    virtual ~AssetRequestComponent() override;

    // Type alias for loader map
    using LoaderMap = std::unordered_map<asset_loader::AssetLoaderID, std::unique_ptr<asset_loader::AssetLoader>>;

    // Type alias for asset source list
    using AssetSourceList = std::vector<std::unique_ptr<asset_loader::AssetSource>>;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;

        // Loaders that need to be registered
        LoaderMap needs_register_loaders;

        // Asset sources that need to be loaded
        AssetSourceList needs_load_asset_sources;

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get loaders that need to be registered
    const LoaderMap& GetLoadersToRegister() const { return needs_register_loaders_; }

    // Extract a loader that needs to be registered
    std::unique_ptr<asset_loader::AssetLoader> ExtractLoaderToRegister(asset_loader::AssetLoaderID id);

    // Add a loader that needs to be registered
    void AddLoaderToRegister(std::unique_ptr<asset_loader::AssetLoader> loader);

    // Clear loaders that need to be registered
    void ClearLoadersToRegister() { needs_register_loaders_.clear(); }

    // Get asset sources that need to be loaded
    const AssetSourceList& GetAssetSourcesToLoad() const { return needs_load_asset_sources_; }

    // Extract an asset source that needs to be loaded
    std::unique_ptr<asset_loader::AssetSource> ExtractAssetSourceToLoad(size_t index);

    // Add an asset source that needs to be loaded
    void AddAssetSourceToLoad(std::unique_ptr<asset_loader::AssetSource> asset_source);

    // Clear asset sources that need to be loaded
    void ClearAssetSourcesToLoad() { needs_load_asset_sources_.clear(); }

    // Get loader IDs that need to be unregistered
    const std::vector<asset_loader::AssetLoaderID>& GetLoaderIDsToUnregister() const { 
        return needs_unregister_loader_ids_; }

    // Add a loader ID that needs to be unregistered
    void AddLoaderIDToUnregister(asset_loader::AssetLoaderID id);

    // Clear loader IDs that need to be unregistered
    void ClearLoaderIDsToUnregister() { needs_unregister_loader_ids_.clear(); }

    // Get asset handle IDs that need to be released
    const std::vector<asset_loader::AssetHandleID>& GetAssetHandleIDsToRelease() const { 
        return needs_release_asset_handle_ids_; }

    // Add an asset handle ID that needs to be released
    void AddAssetHandleIDToRelease(asset_loader::AssetHandleID id);

    // Clear asset handle IDs that need to be released
    void ClearAssetHandleIDsToRelease() { needs_release_asset_handle_ids_.clear(); }

private:
    // Loaders that need to be registered
    LoaderMap needs_register_loaders_;

    // Asset sources that need to be loaded
    AssetSourceList needs_load_asset_sources_;

    // Loaders that need to be unregistered
    std::vector<asset_loader::AssetLoaderID> needs_unregister_loader_ids_;

    // Asset handle IDs that need to be released
    std::vector<asset_loader::AssetHandleID> needs_release_asset_handle_ids_;

};

} // namespace mono_asset_extension