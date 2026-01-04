#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/asset_browser_system.h"

#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_extension/include/asset_request_component.h"
#include "mono_asset_extension/include/asset_loader_set.h"

#include "utility_header/win32.h"
#include "utility_header/logger.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_graphics_extension/include/ui_component.h"

#include "ecs/include/world.h"
#include "mono_entity_archive_extension/include/asset_browser_ui_component.h"
#include "mono_entity_archive_extension/include/asset_browser_drawer.h"

namespace mono_entity_archive_extension
{

AssetBrowserSystem::AssetBrowserSystem(mono_service::ServiceProxyManager& service_proxy_manager) :
    service_proxy_manager_(service_proxy_manager)
{
}

AssetBrowserSystem::~AssetBrowserSystem()
{
}

bool AssetBrowserSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool AssetBrowserSystem::Update(ecs::World& world)
{
    bool result = false;

    // Prepare to load file paths
    std::vector<std::wstring> file_paths_to_load;

    // Prepare to release asset IDs
    std::vector<asset_loader::AssetHandleID> asset_ids_to_release;

    // Iterate through all entities with AssetBrowserUIComponent
    for (const ecs::Entity& entity : world.View(AssetBrowserUIComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "MetaComponent is null!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip inactive entities

        // Get UI component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "UIComponent is null!");

        // Get ui drawer
        mono_graphics_extension::UIDrawer& ui_drawer = ui_component->GetDrawer();

        // Dynamic cast to AssetBrowserDrawer
        AssetBrowserDrawer* asset_browser_drawer
            = dynamic_cast<AssetBrowserDrawer*>(&ui_drawer);
        assert(asset_browser_drawer != nullptr && "UIDrawer is not of type AssetBrowserDrawer!");

        // Check if there is a request to add a new asset
        if (asset_browser_drawer->HasAddAssetRequest())
        {
            // Get absolute open file path
            std::wstring abs_file_path = utility_header::OpenFileDialogWin32();

            if (abs_file_path.empty())
            {
                // Clear the add asset request flag
                asset_browser_drawer->ClearAddAssetRequest();

                // No file selected, skip
                continue;
            }

            // Get working directory
            std::wstring working_dir = utility_header::GetWorkingDir();

            // Get relative file path
            std::wstring rel_file_path = utility_header::GetRelativePath(abs_file_path, working_dir);

            // Add to file paths to load
            file_paths_to_load.emplace_back(rel_file_path);

            // Clear the add asset request flag
            asset_browser_drawer->ClearAddAssetRequest();
        }

        if (asset_browser_drawer->HasReleaseAssetRequest())
        {
            // Get asset release request
            AssetBrowserDrawer::AssetReleaseRequest release_request
                = asset_browser_drawer->GetReleaseAssetRequest();

            // Add to asset IDs to release
            asset_ids_to_release.emplace_back(release_request.asset_id);

            // Clear the release asset request flag
            asset_browser_drawer->ClearReleaseAssetRequest();
        }
    }

    if (asset_request_entity_ == ecs::Entity())
    {
        // Create asset loader set
        mono_asset_extension::AssetLoaderSet asset_loader_set(service_proxy_manager_);

        asset_request_entity_ = world.CreateEntity();

        // Create setup param for asset request component
        std::unique_ptr<mono_asset_extension::AssetRequestComponent::SetupParam> asset_request_param 
            = std::make_unique<mono_asset_extension::AssetRequestComponent::SetupParam>();

        // Store loaders to register
        asset_loader_set.StoreToMap(asset_request_param->needs_register_loaders);

        // Add AssetRequestComponent to the entity
        result = world.AddComponent<mono_asset_extension::AssetRequestComponent>(
            asset_request_entity_, mono_asset_extension::AssetRequestComponentHandle::ID(), std::move(asset_request_param));
        assert(result);
    }

    // Get AssetRequestComponent from the entity
    mono_asset_extension::AssetRequestComponent* asset_request_component
        = world.GetComponent<mono_asset_extension::AssetRequestComponent>(
            asset_request_entity_, mono_asset_extension::AssetRequestComponentHandle::ID());

    if (!file_paths_to_load.empty())
    {
        // Create asset loader set
        mono_asset_extension::AssetLoaderSet asset_loader_set(service_proxy_manager_);

        for (const std::wstring& file_path : file_paths_to_load)
        {
            // Extract asset source to load
            std::unique_ptr<asset_loader::AssetSource> asset_source 
                = asset_loader_set.ExtractAssetSourceToLoad(file_path);

            // Add to asset sources in setup parameter
            asset_request_component->AddAssetSourceToLoad(std::move(asset_source));
        }
    }

    // Add asset IDs to release
    for (const asset_loader::AssetHandleID& asset_id : asset_ids_to_release)
        asset_request_component->AddAssetHandleIDToRelease(asset_id);

    return true; // Success
}

bool AssetBrowserSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID AssetBrowserSystem::GetID() const
{
    return AssetBrowserSystemHandle::ID();
}

} // namespace mono_entity_archive_extension