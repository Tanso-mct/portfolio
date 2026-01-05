#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/project_io_system.h"

#include "utility_header/win32.h"
#include "utility_header/logger.h"

#include "render_graph/include/material_handle_manager.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_service/include/asset_service_view.h"

#include "ecs/include/world.h"
#include "mono_entity_archive_extension/include/import_helper.h"
#include "mono_entity_archive_extension/include/project_io_component.h"
#include "mono_entity_archive_extension/include/editor_enabled_component.h"
#include "mono_meta_extension/include/meta_component.h"

namespace mono_entity_archive_extension
{

ProjectIOSystem::ProjectIOSystem(
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy,
    mono_service::ServiceProxyManager& service_proxy_manager) : 
    entity_archive_service_proxy_(std::move(entity_archive_service_proxy)),
    asset_service_proxy_(std::move(asset_service_proxy)),
    service_proxy_manager_(service_proxy_manager)
{
    assert(entity_archive_service_proxy_ != nullptr && "Entity archive service proxy is null!");
    assert(asset_service_proxy_ != nullptr && "Asset service proxy is null!");
}

ProjectIOSystem::~ProjectIOSystem()
{
}

bool ProjectIOSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool ProjectIOSystem::Update(ecs::World& world)
{
    std::wstring_view project_path;
    bool is_import_requested = false;
    bool is_export_requested = false;
    for (const ecs::Entity& entity : world.View(ProjectIOComponentHandle::ID())())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity who has ProjectIOComponent must have MetaComponent");

        if (!meta_component->IsActiveSelf())
            continue; // Skip inactive entities

        // Get ProjectIOComponent
        ProjectIOComponent* project_io_component
            = world.GetComponent<ProjectIOComponent>(entity, ProjectIOComponentHandle::ID());
        assert(project_io_component != nullptr && "ProjectIOComponent is null!");

        // Get project path
        project_path = project_io_component->GetProjectPath();

        // Check import requests
        if (project_io_component->IsImportRequested())
            is_import_requested = true;

        // Check export requests
        if (project_io_component->IsExportRequested())
            is_export_requested = true;

        // Reset requests
        project_io_component->SetImportRequested(false);
        project_io_component->SetExportRequested(false);
    }

    // Handle import request
    if (is_import_requested)
    {
        // Get absolute open file path
        std::wstring abs_file_path = utility_header::OpenFileDialogWin32();

        if (abs_file_path.empty())
            return true; // User cancelled, nothing to do

        // Get working directory
        std::wstring working_dir = utility_header::GetWorkingDir();

        // Get relative file path
        std::wstring rel_file_path = utility_header::GetRelativePath(abs_file_path, working_dir);

        // Open project entity archive file
        std::ifstream input_file(
            rel_file_path + std::wstring(PROJECT_ENTITY_ARCHIVE_JSON_FILE_PATH));
        if (!input_file.is_open())
        {
            utility_header::ConsoleLog({
                "Failed to open project entity archive file"},
                __FILE__, __LINE__, __FUNCTION__);
            return false; // Failed to open file
        }

        nlohmann::json archive_json;
        input_file >> archive_json;

        // Create entities from exported JSON
        if (!CreateEntitiesFromExportedJSON(world, archive_json, service_proxy_manager_))
        {
            utility_header::ConsoleLog({
                "Failed to create entities from project entity archive file"},
                __FILE__, __LINE__, __FUNCTION__);
            return false; // Failed to create entities
        }
    }

    // Handle export request
    if (is_export_requested)
    {
        // Get absolute open file path
        std::wstring abs_file_path = utility_header::OpenFileDialogWin32();

        if (abs_file_path.empty())
            return true; // User cancelled, nothing to do

        // Get working directory
        std::wstring working_dir = utility_header::GetWorkingDir();

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> entity_archive_command_list
            = entity_archive_service_proxy_->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(entity_archive_command_list.get());
        assert(entity_archive_command_list != nullptr && "Entity archive service command list is null!");

        // Create asset service command list
        std::unique_ptr<mono_service::ServiceCommandList> asset_service_command_list
            = asset_service_proxy_->CreateCommandList();
        mono_asset_service::AssetServiceCommandList* asset_service_command_list_ptr
            = dynamic_cast<mono_asset_service::AssetServiceCommandList*>(asset_service_command_list.get());
        assert(asset_service_command_list != nullptr && "Asset service command list is null!");

        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_view = asset_service_proxy_->CreateView();
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Create export file path
        std::wstring entity_export_file_path = working_dir + std::wstring(PROJECT_ENTITY_ARCHIVE_JSON_FILE_PATH);
        std::wstring asset_export_file_path = working_dir + std::wstring(PROJECT_ASSET_ARCHIVE_JSON_FILE_PATH);
        std::wstring material_export_file_path = working_dir + std::wstring(PROJECT_MATERIAL_ARCHIVE_JSON_FILE_PATH);

        // Prepare entity list for export
        std::vector<ecs::Entity> entities_to_export;

        // Iterate through all editor enabled entities
        for (const ecs::Entity& entity : world.View(EditorEnabledComponentHandle::ID())())
            entities_to_export.emplace_back(entity);

        // Export setup params to the project entity archive
        entity_archive_command_list_ptr->ExportComponentSetupParamsToFile(
            entity_export_file_path, entities_to_export, service_proxy_manager_);

        // Get all loaded asset IDs from asset service view
        std::vector<asset_loader::AssetHandleID> loaded_asset_ids = asset_service_view_ptr->GetLoadedAssetIDs();

        // Export assets to the project asset archive
        asset_service_command_list_ptr->ExportAssetDescriptionsToFile(
            asset_export_file_path, loaded_asset_ids);

        // Get all registered material handle keys
        std::vector<render_graph::MaterialHandleKey> material_handle_keys
            = render_graph::MaterialHandleManager::GetInstance().GetRegisteredMaterialHandleKeys();

        // Prepare material handles for export
        std::vector<render_graph::MaterialHandle> material_handles;
        for (const render_graph::MaterialHandleKey& material_handle_key : material_handle_keys)
        {
            render_graph::MaterialHandle* material_handle_ptr
                = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandle(material_handle_key);
            if (material_handle_ptr != nullptr)
                material_handles.emplace_back(*material_handle_ptr);
        }

        // Export setup params of materials to the project material archive
        entity_archive_command_list_ptr->ExportMaterialSetupParamsToFile(
            material_export_file_path, material_handles, service_proxy_manager_);

        // Submit the command list to the entity archive service
        entity_archive_service_proxy_->SubmitCommandList(std::move(entity_archive_command_list));

        // Submit the command list to the asset service
        asset_service_proxy_->SubmitCommandList(std::move(asset_service_command_list));
    }

    return true; // Success
}

bool ProjectIOSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID ProjectIOSystem::GetID() const
{
    return ProjectIOSystemHandle::ID();
}

} // namespace mono_entity_archive_extension