#include "mono_forge/src/pch.h"
#include "mono_forge/include/initial_scene.h"

#include "utility_header/file_loader.h"
#include "utility_header/logger.h"

#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_service/include/asset_service_command_list.h"
#include "mono_asset_extension/include/asset_request_component.h"
#include "mono_asset_extension/include/asset_loader_set.h"
#include "geometry/include/triangle.h"
#include "mono_forge_model/include/mfm.h"

#include "mono_meta_extension/include/meta_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"

#include "mono_transform_service/include/transform_service_view.h"
#include "mono_transform_extension/include/transform_component.h"

#include "mono_window_service/include/window_service_view.h"
#include "mono_window_extension/include/window_component.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_graphics_service/include/graphics_service_view.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_extension/include/graphics_system.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/directional_light_component.h"
#include "mono_graphics_extension/include/ambient_light_component.h"
#include "mono_graphics_extension/include/point_light_component.h"

#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_entity_archive_extension/include/material_additional.h"
#include "mono_entity_archive_extension/include/editor_enabled_component.h"
#include "mono_entity_archive_extension/include/hierarchy_ui_component.h"
#include "mono_entity_archive_extension/include/inspector_ui_component.h"
#include "mono_entity_archive_extension/include/asset_browser_ui_component.h"
#include "mono_entity_archive_extension/include/transform_manipulator_ui_component.h"
#include "mono_entity_archive_extension/include/menu_bar_ui_component.h"
#include "mono_entity_archive_extension/include/material_editor_ui_component.h"
#include "mono_entity_archive_extension/include/hierarchy_drawer.h"
#include "mono_entity_archive_extension/include/inspector_drawer.h"
#include "mono_entity_archive_extension/include/asset_browser_drawer.h"
#include "mono_entity_archive_extension/include/manipulator_drawer.h"
#include "mono_entity_archive_extension/include/menu_bar_drawer.h"
#include "mono_entity_archive_extension/include/material_editor_drawer.h"

#include "mono_forge/include/assets.h"
#include "mono_forge/include/materials.h"

namespace mono_forge
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
    // Get service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Create entity with asset request component to load asset
    {
        asset_request_entity_ = world.CreateEntity();

        std::unique_ptr<mono_asset_extension::AssetRequestComponent::SetupParam> asset_request_param 
            = std::make_unique<mono_asset_extension::AssetRequestComponent::SetupParam>();

        // Create asset loader set
        mono_asset_extension::AssetLoaderSet asset_loader_set(service_proxy_manager_);

        // Store loaders to register
        asset_loader_set.StoreToMap(asset_request_param->needs_register_loaders);

        // Add empty texture asset source
        {
            // Create texture asset source data
            std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data 
                = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

            // Set texture info for empty texture asset
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

        // Add box mesh asset source
        {
            std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data 
                = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy->Clone());

            // Set MFM data path
            mesh_source_data->SetFilePath(BOX_MESH_ASSET_PATH);

            // Create asset description for box mesh asset
            std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    BoxMeshAssetHandle::ID(), 
                    mono_asset_extension::MeshAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> mesh_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            mesh_asset_source->source_data = std::move(mesh_source_data);
            mesh_asset_source->description = std::move(mesh_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(mesh_asset_source));
        }

        // Add marble bust mesh asset source
        {
            std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data 
                = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy->Clone());

            // Set MFM data path
            mesh_source_data->SetFilePath(MARBLE_BUST_MODEL_PATH);

            // Create asset description for marble bust mesh asset
            std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    MarbleBustMeshAssetHandle::ID(), 
                    mono_asset_extension::MeshAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> mesh_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            mesh_asset_source->source_data = std::move(mesh_source_data);
            mesh_asset_source->description = std::move(mesh_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(mesh_asset_source));
        }

        // Add marble bust albedo texture asset source
        {
            std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data 
                = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

            // Set file path
            texture_source_data->SetFilePath(MARBLE_BUST_ALBEDO_TEXTURE_PATH);

            // Create asset description for marble bust albedo texture asset
            std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    MarbleBustAlbedoTextureAssetHandle::ID(),
                    mono_asset_extension::TextureAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            texture_asset_source->source_data = std::move(texture_source_data);
            texture_asset_source->description = std::move(texture_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
        }

        // Add marble bust normal texture asset source
        {
            std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data 
                = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

            // Set file path
            texture_source_data->SetFilePath(MARBLE_BUST_NORMAL_TEXTURE_PATH);

            // Create asset description for marble bust normal texture asset
            std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    MarbleBustNormalTextureAssetHandle::ID(),
                    mono_asset_extension::TextureAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            texture_asset_source->source_data = std::move(texture_source_data);
            texture_asset_source->description = std::move(texture_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
        }

        // Add marble bust AO texture asset source
        {
            std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data
                = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

            // Set file path
            texture_source_data->SetFilePath(MARBLE_BUST_AO_TEXTURE_PATH);

            // Create asset description for marble bust AO texture asset
            std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    MarbleBustAOTextureAssetHandle::ID(),
                    mono_asset_extension::TextureAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            texture_asset_source->source_data = std::move(texture_source_data);
            texture_asset_source->description = std::move(texture_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
        }

        // Add marble bust roughness texture asset source
        {
            std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data
                = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy->Clone());

            // Set file path
            texture_source_data->SetFilePath(MARBLE_BUST_ROUGHNESS_TEXTURE_PATH);

            // Create asset description for marble bust roughness texture asset
            std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    MarbleBustRoughnessTextureAssetHandle::ID(),
                    mono_asset_extension::TextureAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            texture_asset_source->source_data = std::move(texture_source_data);
            texture_asset_source->description = std::move(texture_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
        }

        // Add floor mesh asset source
        {
            std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data 
                = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy->Clone());

            // Set MFM data path
            mesh_source_data->SetFilePath(FLOOR_MODEL_PATH);

            // Create asset description for floor mesh asset
            std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    FloorMeshAssetHandle::ID(), 
                    mono_asset_extension::MeshAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> mesh_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            mesh_asset_source->source_data = std::move(mesh_source_data);
            mesh_asset_source->description = std::move(mesh_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(mesh_asset_source));
        }

        // Add AssetRequestComponent to the entity
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
    if (!asset_service_view->IsAssetLoaded(EmptyTextureAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(BoxMeshAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(MarbleBustMeshAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(MarbleBustAlbedoTextureAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(MarbleBustNormalTextureAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(MarbleBustAOTextureAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(MarbleBustRoughnessTextureAssetHandle::ID())) return false; // Not loaded
    if (!asset_service_view->IsAssetLoaded(FloorMeshAssetHandle::ID())) return false; // Not loaded

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
    // Get service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
    service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
        transform_service_proxy = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID()).Clone();
        entity_archive_service_proxy = manager.GetServiceProxy(mono_entity_archive_service::EntityArchiveServiceHandle::ID()).Clone();
        asset_service_proxy = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
    });

    // Get material handle manager
    render_graph::MaterialHandleManager& material_handle_manager
        = render_graph::MaterialHandleManager::GetInstance();

    mono_window_extension::WindowComponent* window_component 
        = world.GetComponent<mono_window_extension::WindowComponent>(
            window_entity_, mono_window_extension::WindowComponentHandle::ID());
    assert(window_component != nullptr && "Window component is null!");

    // Camera
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Main Camera";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Main Camera";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, 0.0f, -2.0f);

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, 0.0f, -2.0f);

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Camera component
        {
            std::unique_ptr<mono_graphics_extension::CameraComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::CameraComponent::SetupParam>();
            param->fov_y = 60.0f;
            param->aspect_ratio 
                = static_cast<float>(window_component->GetClientWidth())
                / static_cast<float>(window_component->GetClientHeight());
            param->near_z = 0.1f;
            param->far_z = 1000.0f;

            bool result = world.AddComponent<mono_graphics_extension::CameraComponent>(
                entity, mono_graphics_extension::CameraComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::CameraComponent::SetupParam>();
            param->fov_y = 60.0f;
            param->aspect_ratio 
                = static_cast<float>(window_component->GetClientWidth())
                / static_cast<float>(window_component->GetClientHeight());
            param->near_z = 0.1f;
            param->far_z = 1000.0f;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::CameraComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Marble bust
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Marble Bust";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Marble Bust";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, -0.8f, 0.0f);
            param->rotation = XMFLOAT3(0.0f, 180.0f, 0.0f);
            param->scale = XMFLOAT3(0.03f, 0.03f, 0.03f);

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, -0.8f, 0.0f);
            param->rotation = XMFLOAT3(0.0f, 180.0f, 0.0f);
            param->scale = XMFLOAT3(0.03f, 0.03f, 0.03f);

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Renderable component
        {
            // Create asset service view to get mesh asset handle
            std::unique_ptr<mono_service::ServiceView> service_view = asset_service_proxy->CreateView();

            mono_asset_service::AssetServiceView* asset_service_view
                = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
            assert(asset_service_view != nullptr && "Asset service view is null!");

            // Get marble bust mesh asset
            const asset_loader::Asset& mesh_asset
                = asset_service_view->GetAsset(MarbleBustMeshAssetHandle::ID());
            const mono_asset_extension::MeshAsset* mesh_asset_ptr
                = dynamic_cast<const mono_asset_extension::MeshAsset*>(&mesh_asset);
            assert(mesh_asset_ptr != nullptr && "Failed to get marble bust mesh asset!");

            // Create renderable component setup param
            std::unique_ptr<mono_graphics_extension::RenderableComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
            param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
            param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
            param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();
            
            std::vector<const render_graph::MaterialHandle*> material_handles;
            material_handles.resize(mesh_asset_ptr->GetIndexCounts()->size());
            for (int i = 0; i < material_handles.size(); ++i)
                material_handles[i] = material_handle_manager.GetMaterialHandle(MarbleBustPhongMaterialHandleKey().Value());
            param->mesh_set_.material_handles = std::move(material_handles);

            param->mesh_set_.mesh_asset_handle = MarbleBustMeshAssetHandle::ID();

            bool result = world.AddComponent<mono_graphics_extension::RenderableComponent>(
                entity, mono_graphics_extension::RenderableComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
            param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
            param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
            param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();

            std::vector<const render_graph::MaterialHandle*> archive_material_handles;
            archive_material_handles.resize(mesh_asset_ptr->GetIndexCounts()->size());
            for (int i = 0; i < archive_material_handles.size(); ++i)
                archive_material_handles[i] 
                    = material_handle_manager.GetMaterialHandle(MarbleBustPhongMaterialHandleKey().Value());
            param->mesh_set_.material_handles = std::move(archive_material_handles);

            param->mesh_set_.mesh_asset_handle = MarbleBustMeshAssetHandle::ID();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::RenderableComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Floor
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Floor";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Floor";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, -1.0f, 0.0f);
            param->scale = XMFLOAT3(10.0f, 1.0f, 10.0f);

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(0.0f, -1.0f, 0.0f);
            param->scale = XMFLOAT3(10.0f, 1.0f, 10.0f);

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Renderable component
        {
            // Create asset service view to get mesh asset handle
            std::unique_ptr<mono_service::ServiceView> service_view = asset_service_proxy->CreateView();

            mono_asset_service::AssetServiceView* asset_service_view
                = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
            assert(asset_service_view != nullptr && "Asset service view is null!");

            // Get floor mesh asset
            const asset_loader::Asset& mesh_asset
                = asset_service_view->GetAsset(FloorMeshAssetHandle::ID());
            const mono_asset_extension::MeshAsset* mesh_asset_ptr
                = dynamic_cast<const mono_asset_extension::MeshAsset*>(&mesh_asset);
            assert(mesh_asset_ptr != nullptr && "Failed to get floor mesh asset!");

            std::unique_ptr<mono_graphics_extension::RenderableComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
            param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
            param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
            param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();

            std::vector<const render_graph::MaterialHandle*> material_handles;
            material_handles.resize(mesh_asset_ptr->GetIndexCounts()->size());
            for (int i = 0; i < material_handles.size(); ++i)
                material_handles[i] = material_handle_manager.GetMaterialHandle(FloorLambertMaterialHandleKey().Value());
            param->mesh_set_.material_handles = std::move(material_handles);

            param->mesh_set_.mesh_asset_handle = FloorMeshAssetHandle::ID();

            bool result = world.AddComponent<mono_graphics_extension::RenderableComponent>(
                entity, mono_graphics_extension::RenderableComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
            param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
            param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
            param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();

            std::vector<const render_graph::MaterialHandle*> archive_material_handles;
            archive_material_handles.resize(mesh_asset_ptr->GetIndexCounts()->size());
            for (int i = 0; i < archive_material_handles.size(); ++i)
                archive_material_handles[i] = material_handle_manager.GetMaterialHandle(FloorLambertMaterialHandleKey().Value());
            param->mesh_set_.material_handles = std::move(archive_material_handles);

            param->mesh_set_.mesh_asset_handle = FloorMeshAssetHandle::ID();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::RenderableComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Directional light
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Directional Light";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Directional Light";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->rotation = XMFLOAT3(60.0f, 90.0f, 0.0f);

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->rotation = XMFLOAT3(60.0f, 90.0f, 0.0f);

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Directional Light component
        {
            std::unique_ptr<mono_graphics_extension::DirectionalLightComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::DirectionalLightComponent::SetupParam>();

            bool result = world.AddComponent<mono_graphics_extension::DirectionalLightComponent>(
                entity, mono_graphics_extension::DirectionalLightComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone(), mono_graphics_extension::BACK_BUFFER_COUNT);
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::DirectionalLightComponent::SetupParam>();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::DirectionalLightComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Ambient light
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Ambient Light";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Ambient Light";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Ambient Light component
        {
            std::unique_ptr<mono_graphics_extension::AmbientLightComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::AmbientLightComponent::SetupParam>();

            bool result = world.AddComponent<mono_graphics_extension::AmbientLightComponent>(
                entity, mono_graphics_extension::AmbientLightComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone(), mono_graphics_extension::BACK_BUFFER_COUNT);
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::AmbientLightComponent::SetupParam>();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::AmbientLightComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Point light
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Point Light";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Point Light";
            param->active_self = true;

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Transform component
        {
            std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
                = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(2.0f, 2.0f, -2.0f);

            bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(param), transform_service_proxy->Clone());
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
            param->position = XMFLOAT3(2.0f, 2.0f, -2.0f);

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_transform_extension::TransformComponentHandle::ID(), std::move(param));
        }

        // Point Light component
        {
            std::unique_ptr<mono_graphics_extension::PointLightComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::PointLightComponent::SetupParam>();

            bool result = world.AddComponent<mono_graphics_extension::PointLightComponent>(
                entity, mono_graphics_extension::PointLightComponentHandle::ID(),
                std::move(param), graphics_service_proxy->Clone(), mono_graphics_extension::BACK_BUFFER_COUNT);
            assert(result);

            // Create setup param for entity archive service again because it was moved
            param = std::make_unique<mono_graphics_extension::PointLightComponent::SetupParam>();

            // Add setup param to entity archive service if editable
            mono_entity_archive_service::AddSetupParam(
                service_proxy_manager_, entity,
                mono_graphics_extension::PointLightComponentHandle::ID(), std::move(param));
        }

        // Editor enabled component
        {
            std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
                entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Hierarchy UI
    ecs::Entity hierarchy_entity = world.CreateEntity();
    {
        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Hierarchy";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                hierarchy_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                hierarchy_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::HierarchyDrawer>();

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                hierarchy_entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Hierarchy UI component
        {
            std::unique_ptr<mono_entity_archive_extension::HierarchyUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::HierarchyUIComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::HierarchyUIComponent>(
                hierarchy_entity, mono_entity_archive_extension::HierarchyUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Inspector UI
    ecs::Entity inspector_entity = world.CreateEntity();
    {
        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Inspector";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                inspector_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                inspector_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
                
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::InspectorDrawer>(
                service_proxy_manager_, entity_archive_service_proxy->Clone());

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                inspector_entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Inspector UI component
        {
            std::unique_ptr<mono_entity_archive_extension::InspectorUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::InspectorUIComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::InspectorUIComponent>(
                inspector_entity, mono_entity_archive_extension::InspectorUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Asset browser UI
    ecs::Entity asset_browser_entity = world.CreateEntity();
    {
        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Asset Browser";
            param->active_self = false;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                asset_browser_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                asset_browser_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
                
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::AssetBrowserDrawer>(
                asset_service_proxy->Clone());
            param->ui_drawer->SetShowWindow(false);

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                asset_browser_entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Asset Browser UI component
        {
            std::unique_ptr<mono_entity_archive_extension::AssetBrowserUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::AssetBrowserUIComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::AssetBrowserUIComponent>(
                asset_browser_entity, mono_entity_archive_extension::AssetBrowserUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Transform manipulator
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Transform Manipulator";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
                
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::ManipulatorDrawer>();

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Transform Manipulator ui component
        {
            std::unique_ptr<mono_entity_archive_extension::TransformManipulatorUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::TransformManipulatorUIComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::TransformManipulatorUIComponent>(
                entity, mono_entity_archive_extension::TransformManipulatorUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Material editor
    ecs::Entity material_editor_entity = world.CreateEntity();
    {
        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Material Editor";
            param->active_self = false;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                material_editor_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                material_editor_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
                
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::MaterialEditorDrawer>(
                entity_archive_service_proxy->Clone(), graphics_service_proxy->Clone(), service_proxy_manager_);
            param->ui_drawer->SetShowWindow(false);

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                material_editor_entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Material Editor ui component
        {
            std::unique_ptr<mono_entity_archive_extension::MaterialEditorUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::MaterialEditorUIComponent::SetupParam>();

            bool result = world.AddComponent<mono_entity_archive_extension::MaterialEditorUIComponent>(
                material_editor_entity, mono_entity_archive_extension::MaterialEditorUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
    }

    // Menu bar
    {
        ecs::Entity entity = world.CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Menu Bar";
            param->active_self = true;

            bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = InitialScene::ID();

            bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            assert(result);
        }

        // UI component
        {
            std::unique_ptr<mono_graphics_extension::UIComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::UIComponent::SetupParam>();
                
            param->ui_drawer = std::make_unique<mono_entity_archive_extension::MenuBarDrawer>();

            bool result = world.AddComponent<mono_graphics_extension::UIComponent>(
                entity, mono_graphics_extension::UIComponentHandle::ID(),std::move(param));
            assert(result);
        }

        // Menu Bar ui component
        {
            std::unique_ptr<mono_entity_archive_extension::MenuBarUIComponent::SetupParam> param
                = std::make_unique<mono_entity_archive_extension::MenuBarUIComponent::SetupParam>();
            param->hierarchy_entity_ = hierarchy_entity;
            param->inspector_entity_ = inspector_entity;
            param->asset_browser_entity_ = asset_browser_entity;
            param->material_editor_entity_ = material_editor_entity;

            bool result = world.AddComponent<mono_entity_archive_extension::MenuBarUIComponent>(
                entity, mono_entity_archive_extension::MenuBarUIComponentHandle::ID(), std::move(param));
            assert(result);
        }
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
    // asset_command_list_ptr->ReleaseLoadingAssetSource(MarbleBustMeshAssetHandle::ID());
    // asset_command_list_ptr->ReleaseLoadingAssetSource(MarbleBustAlbedoTextureAssetHandle::ID());
    // asset_command_list_ptr->ReleaseLoadingAssetSource(MarbleBustNormalTextureAssetHandle::ID());
    // asset_command_list_ptr->ReleaseLoadingAssetSource(MarbleBustAOTextureAssetHandle::ID());
    // asset_command_list_ptr->ReleaseLoadingAssetSource(MarbleBustRoughnessTextureAssetHandle::ID());
    // asset_command_list_ptr->ReleaseLoadingAssetSource(FloorMeshAssetHandle::ID());

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

    {
        // Get empty texture asset
        const asset_loader::Asset& empty_texture_asset
            = asset_service_view_ptr->GetAsset(EmptyTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);
        assert(empty_texture_asset_ptr != nullptr && "Failed to get empty texture asset!");

        // Get marble bust albedo texture asset
        const asset_loader::Asset& marble_bust_albedo_texture_asset
            = asset_service_view_ptr->GetAsset(MarbleBustAlbedoTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* marble_bust_albedo_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_albedo_texture_asset);
        assert(marble_bust_albedo_texture_asset_ptr != nullptr && "Failed to get marble bust albedo texture asset!");

        // Get marble bust normal texture asset
        const asset_loader::Asset& marble_bust_normal_texture_asset
            = asset_service_view_ptr->GetAsset(MarbleBustNormalTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* marble_bust_normal_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_normal_texture_asset);
        assert(marble_bust_normal_texture_asset_ptr != nullptr && "Failed to get marble bust normal texture asset!");

        // Get marble bust AO texture asset
        const asset_loader::Asset& marble_bust_ao_texture_asset
            = asset_service_view_ptr->GetAsset(MarbleBustAOTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* marble_bust_ao_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_ao_texture_asset);
        assert(marble_bust_ao_texture_asset_ptr != nullptr && "Failed to get marble bust AO texture asset!");

        // Get marble bust roughness texture asset
        const asset_loader::Asset& marble_bust_roughness_texture_asset
            = asset_service_view_ptr->GetAsset(MarbleBustRoughnessTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* marble_bust_roughness_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_roughness_texture_asset);
        assert(marble_bust_roughness_texture_asset_ptr != nullptr && "Failed to get marble bust roughness texture asset!");

        // Create marble bust lambert material setup parameter
        std::unique_ptr<render_graph::PhongMaterial::SetupParam> phong_material_param
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        phong_material_param->base_color = MARBLE_BUST_DEFAULT_BASE_COLOR;
        phong_material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE;
        phong_material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
        phong_material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_TEXTURE;
        phong_material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        phong_material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE;
        phong_material_param->roughness_value = MARBLE_BUST_DEFAULT_ROUGHNESS_VALUE;
        phong_material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        phong_material_param->metalness_value = MARBLE_BUST_DEFAULT_METALNESS_VALUE;
        phong_material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        phong_material_param->emission_color = MARBLE_BUST_DEFAULT_EMISSION_COLOR;
        phong_material_param->albedo_texture_handle = marble_bust_albedo_texture_asset_ptr->GetTextureHandle();
        phong_material_param->normal_texture_handle = marble_bust_normal_texture_asset_ptr->GetTextureHandle();
        phong_material_param->ao_texture_handle = marble_bust_ao_texture_asset_ptr->GetTextureHandle();
        phong_material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->roughness_texture_handle = marble_bust_roughness_texture_asset_ptr->GetTextureHandle();
        phong_material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(
            MarbleBustPhongMaterialHandleKey().Value(), render_graph::MaterialHandle(), "Marble Bust Phong");

        // Create marble bust phong material
        graphics_command_list_ptr->CreateMaterial<render_graph::PhongMaterial>(
            material_handle_manager.GetMaterialHandle(MarbleBustPhongMaterialHandleKey().Value()), 
            std::move(phong_material_param));

        // Create marble bust lambert material setup parameter again because it was moved
        phong_material_param
            = std::make_unique<render_graph::PhongMaterial::SetupParam>();
        phong_material_param->base_color = MARBLE_BUST_DEFAULT_BASE_COLOR;
        phong_material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE;
        phong_material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
        phong_material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_TEXTURE;
        phong_material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
        phong_material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE;
        phong_material_param->roughness_value = MARBLE_BUST_DEFAULT_ROUGHNESS_VALUE;
        phong_material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
        phong_material_param->metalness_value = MARBLE_BUST_DEFAULT_METALNESS_VALUE;
        phong_material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
        phong_material_param->emission_color = MARBLE_BUST_DEFAULT_EMISSION_COLOR;
        phong_material_param->albedo_texture_handle = marble_bust_albedo_texture_asset_ptr->GetTextureHandle();
        phong_material_param->normal_texture_handle = marble_bust_normal_texture_asset_ptr->GetTextureHandle();
        phong_material_param->ao_texture_handle = marble_bust_ao_texture_asset_ptr->GetTextureHandle();
        phong_material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->roughness_texture_handle = marble_bust_roughness_texture_asset_ptr->GetTextureHandle();
        phong_material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        phong_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Create phong material additional setup param for entity archive service
        std::unique_ptr<mono_entity_archive_extension::PhongMaterialAdditionalSetupParam> phong_material
            = std::make_unique<mono_entity_archive_extension::PhongMaterialAdditionalSetupParam>(
                MarbleBustAlbedoTextureAssetHandle::ID(), // Albedo
                MarbleBustNormalTextureAssetHandle::ID(), // Normal
                MarbleBustAOTextureAssetHandle::ID(), // AO
                EmptyTextureAssetHandle::ID(), // Specular
                MarbleBustRoughnessTextureAssetHandle::ID(), // Roughness
                EmptyTextureAssetHandle::ID(), // Metalness
                EmptyTextureAssetHandle::ID()); // Emission

        // Add setup param to entity archive service
        entity_archive_command_list_ptr->AddSetupParam(
            material_handle_manager.GetMaterialHandle(MarbleBustPhongMaterialHandleKey().Value()), 
            std::make_unique<material_editor::SetupParamWrapper>(
                std::move(phong_material_param),
                std::move(phong_material)));
    }

    {
        // Get empty texture asset
        const asset_loader::Asset& empty_texture_asset
            = asset_service_view_ptr->GetAsset(EmptyTextureAssetHandle::ID());
        const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
            = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);
        assert(empty_texture_asset_ptr != nullptr && "Failed to get empty texture asset!");

        // Create floor lambert material setup parameter
        std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = FLOOR_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = FLOOR_DEFAULT_EMISSION_COLOR;
        lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
        lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

        // Register material handle
        material_handle_manager.RegisterMaterialHandle(
            FloorLambertMaterialHandleKey().Value(), render_graph::MaterialHandle(), "Floor Lambert");

        // Create floor lambert material
        graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
            material_handle_manager.GetMaterialHandle(FloorLambertMaterialHandleKey().Value()), 
            std::move(lambert_material_param));

        // Create floor lambert material setup parameter again because it was moved
        lambert_material_param
            = std::make_unique<render_graph::LambertMaterial::SetupParam>();
        lambert_material_param->base_color = FLOOR_DEFAULT_BASE_COLOR;
        lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
        lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
        lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
        lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
        lambert_material_param->emission_color = FLOOR_DEFAULT_EMISSION_COLOR;
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
            material_handle_manager.GetMaterialHandle(FloorLambertMaterialHandleKey().Value()), 
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

} // namespace mono_forge