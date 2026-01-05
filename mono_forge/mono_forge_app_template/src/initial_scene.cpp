#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/initial_scene.h"

#include "utility_header/file_loader.h"
#include "utility_header/logger.h"
#include "utility_header/win32.h"

#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_extension/include/asset_request_component.h"
#include "mono_asset_extension/include/asset_loader_set.h"
#include "mono_asset_extension/include/import_helper.h"
#include "mono_forge_model/include/mfm.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"

#include "mono_window_service/include/window_service_view.h"
#include "mono_window_extension/include/window_component.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_graphics_service/include/graphics_service_view.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_extension/include/graphics_system.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_entity_archive_extension/include/project_io_system.h"
#include "mono_entity_archive_extension/include/material_additional.h"
#include "mono_entity_archive_extension/include/import_helper.h"

#include "mono_forge_app_template/include/assets.h"
#include "mono_forge_app_template/include/materials.h"

namespace mono_forge_app_template
{

InitialScene::InitialScene(
    mono_service::ServiceProxyManager& service_proxy_manager, 
    const ecs::Entity& scene_entity, const ecs::Entity& window_entity) :
    service_proxy_manager_(service_proxy_manager),
    scene_entity_(scene_entity),
    window_entity_(window_entity)
{
}

bool InitialScene::LoadAssets(ecs::World& world)
{
    asset_request_entity_ = world.CreateEntity();

    // Create asset request setup parameter
    std::unique_ptr<mono_asset_extension::AssetRequestComponent::SetupParam> asset_request_param 
        = std::make_unique<mono_asset_extension::AssetRequestComponent::SetupParam>();

    // Create asset loader set
    mono_asset_extension::AssetLoaderSet asset_loader_set(service_proxy_manager_);

    // Store loaders to register
    asset_loader_set.StoreToMap(asset_request_param->needs_register_loaders);

    // Get working directory
    std::wstring working_dir = utility_header::GetWorkingDir();

    // Open project entity archive file
    std::ifstream input_file(
        working_dir + std::wstring(mono_entity_archive_extension::PROJECT_ASSET_ARCHIVE_JSON_FILE_PATH));
    if (!input_file.is_open())
    {
        utility_header::ConsoleLog({
            "Failed to open project entity archive file"},
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Failed to open file
    }

    nlohmann::json archive_json;
    input_file >> archive_json;

    // Create asset sources from exported JSON
    if (!mono_asset_extension::CreateAssetSourcesFromExportedJSON(
        archive_json, service_proxy_manager_, asset_request_param->needs_load_asset_sources))
    {
        utility_header::ConsoleLog({
            "Failed to create asset sources from project asset archive file"},
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Failed to create asset sources
    }

    // Get service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Add empty texture asset source
    {
        // Create texture asset source data
        std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data 
            = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

        // Set texture info for empty texture asset
        texture_source_data->SetName("Empty");
        mono_asset_extension::TextureAssetSourceData::TextureInfo texture_info = {};
        texture_info.width = 1;
        texture_info.height = 1;
        texture_info.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        texture_source_data->SetTextureInfo(texture_info);

        // Create asset description for empty texture asset
        std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
            = std::make_unique<asset_loader::AssetDescription>(
                EmptyTextureAssetHandle::ID(),
                mono_asset_extension::TextureAssetLoader::ID());

        // Create asset source
        std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
            = std::make_unique<asset_loader::AssetSource>();
        texture_asset_source->source_data = std::move(texture_source_data);
        texture_asset_source->description = std::move(texture_asset_desc);

        // Add to asset sources in setup parameter
        asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
    }

    // Get all asset handle id from asset sources
    use_asset_handles_.clear();
    for (const std::unique_ptr<asset_loader::AssetSource>& asset_source : asset_request_param->needs_load_asset_sources)
        use_asset_handles_.emplace_back(asset_source->description->GetHandleID());

    // Add asset request component to the entity
    {
        bool result = world.AddComponent<mono_asset_extension::AssetRequestComponent>(
            asset_request_entity_, mono_asset_extension::AssetRequestComponentHandle::ID(), std::move(asset_request_param));
        assert(result);
    }

    return true; // Success
}

bool InitialScene::IsAssetsLoaded(ecs::World& world) const
{
    // Get service proxies
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        asset_service_proxy = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
    });

    // Create asset service view
    std::unique_ptr<mono_service::ServiceView> service_view = asset_service_proxy->CreateView();

    // Cast to asset service view
    mono_asset_service::AssetServiceView* asset_service_view
        = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
    assert(asset_service_view != nullptr && "Asset service view is null!");

    // Check if all assets are loaded
    for (const asset_loader::AssetHandleID& handle_id : use_asset_handles_)
    {
        if (!asset_service_view->IsAssetLoaded(handle_id))
            return false; // Not loaded
    }

    return true; // Loaded
}

bool InitialScene::ReleaseAssets(ecs::World& world)
{
    return true; // Success
}

bool InitialScene::IsAssetsReleased(ecs::World& world) const
{
    return true; // Released
}

bool InitialScene::Create(ecs::World& world)
{
    bool result = false;

    result = CreateMaterials(world);
    assert(result);

    result = CreateEntities(world);
    assert(result);

    return true; // Success
}

bool InitialScene::Destroy(ecs::World& world)
{
    bool result = false;

    result = DestroyEntities(world);
    assert(result);

    result = DestroyMaterials(world);
    assert(result);

    return true; // Success
}

bool InitialScene::CreateEntities(ecs::World& world)
{
    // Get working directory
    std::wstring working_dir = utility_header::GetWorkingDir();

    // Open project entity archive file
    std::ifstream input_file(
        working_dir + std::wstring(mono_entity_archive_extension::PROJECT_ENTITY_ARCHIVE_JSON_FILE_PATH));
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
    std::vector<ecs::Entity> created_entities;
    if (
        !mono_entity_archive_extension::CreateEntitiesFromExportedJSON(
            world, archive_json, service_proxy_manager_, &created_entities))
    {
        utility_header::ConsoleLog({
            "Failed to create entities from project entity archive file"},
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Failed to create entities
    }

    // Iterate created entities to add scene tag component
    for (const ecs::Entity& entity : created_entities)
    {
        // Add scene tag component to the entity
        std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> scene_tag_param
            = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
        scene_tag_param->scene_id = InitialScene::ID();

        bool add_result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
            entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(scene_tag_param));
        assert(add_result);
    }

    return true; // Success
}

bool InitialScene::DestroyEntities(ecs::World& world)
{
    for (const ecs::Entity& entity : world.View(mono_scene_extension::SceneTagComponentHandle::ID())())
    {
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Scene tag component is null!");

        if (scene_tag_component->GetSceneID() != InitialScene::ID())
            continue; // Skip if not this scene

        // Check entity is not window entity
        if (world.HasComponent(entity, mono_window_extension::WindowComponentHandle::ID()))
            continue; // Skip window entity

        bool result = world.DestroyEntity(entity);
        assert(result);
    }

    return true; // Success
}

bool InitialScene::CreateMaterials(ecs::World& world)
{
    // Get service proxies
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        asset_service_proxy = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
        entity_archive_service_proxy = manager.GetServiceProxy(mono_entity_archive_service::EntityArchiveServiceHandle::ID()).Clone();
    });

    // Create asset service view
    std::unique_ptr<mono_service::ServiceView> asset_view = asset_service_proxy->CreateView();
    mono_asset_service::AssetServiceView* asset_service_view_ptr
        = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_view.get());
    assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
    assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

    // Create entity archive service command list
    std::unique_ptr<mono_service::ServiceCommandList> entity_archive_command_list = entity_archive_service_proxy->CreateCommandList();
    mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list_ptr
        = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(entity_archive_command_list.get());
    assert(entity_archive_command_list_ptr != nullptr && "Entity archive service command list is null!");

    // All assets are loaded, destroy loading asset sources
    // asset_command_list_ptr->ReleaseLoadingAssetSource(EmptyTextureAssetHandle::ID());

    // Get material handle manager
    render_graph::MaterialHandleManager& material_handle_manager
        = render_graph::MaterialHandleManager::GetInstance();

    {
        // Get empty texture asset
        const asset_loader::Asset& empty_texture_asset
            = asset_service_view_ptr->GetAsset(EmptyTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);

        // Create box lambert material setup parameter
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(
            BoxLambertMaterialHandleKey().Value(), render_graph::MaterialHandle(), "Box Lambert");

        // Create box lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(BoxLambertMaterialHandleKey().Value()), 
            std::move(lambert_material_param));

        // Create setup param for entity archive service again because it was moved
        lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = BOX_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = BOX_DEFAULT_EMISSION_COLOR;
        lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Create lambert material additional setup param for entity archive service
        std::unique_ptr<mono_entity_archive_extension::LambertMaterialAdditionalSetupParam> lambert_material_additional_param
            = std::make_unique<mono_entity_archive_extension::LambertMaterialAdditionalSetupParam>(
                EmptyTextureAssetHandle::ID(), // Albedo
                EmptyTextureAssetHandle::ID(), // Normal
                EmptyTextureAssetHandle::ID(), // AO
                EmptyTextureAssetHandle::ID()); // Emission

        // Add setup param to entity archive service
        entity_archive_command_list_ptr->AddSetupParam(
            material_handle_manager.GetMaterialHandle(BoxLambertMaterialHandleKey().Value()), 
            std::make_unique<material_editor::SetupParamWrapper>(
                std::move(lambert_material_param),
                std::move(lambert_material_additional_param)));
    }

    // Submit graphics service command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

    // Submit entity archive service command list
    entity_archive_service_proxy->SubmitCommandList(std::move(entity_archive_command_list));

    // Loading is complete, remove asset request component entity
    world.RemoveComponent(
        asset_request_entity_, mono_asset_extension::AssetRequestComponentHandle::ID());

    return true; // Success
}

bool InitialScene::DestroyMaterials(ecs::World& world)
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = graphics_service_proxy->CreateCommandList();

    // Cast to graphics service command list
    mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
    assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

    // Get material handle manager
    render_graph::MaterialHandleManager& material_handle_manager
        = render_graph::MaterialHandleManager::GetInstance();

    // Destroy materials
    graphics_command_list_ptr->DestroyMaterial(material_handle_manager.GetMaterialHandle(MarbleBustPhongMaterialHandleKey().Value()));
    graphics_command_list_ptr->DestroyMaterial(material_handle_manager.GetMaterialHandle(FloorLambertMaterialHandleKey().Value()));

    // Submit graphics service command list
    graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

    return true; // Success
}

} // namespace mono_forge_app_template