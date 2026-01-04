#include "mono_graphics_extension_test/pch.h"

using namespace DirectX;

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

#include "ecs/include/world.h"
#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

#include "geometry/include/triangle.h"
#include "mono_asset_service/include/asset_service.h"
#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/allocator_factory.h"
#include "mono_asset_extension/include/asset_request_component.h"
#include "mono_asset_extension/include/asset_request_processor_system.h"
#include "mono_asset_extension/include/mesh_asset.h"
#include "mono_asset_extension/include/texture_asset.h"

#include "mono_meta_extension/include/allocator_factory.h"
#include "mono_meta_extension/include/meta_component.h"
#include "mono_meta_extension/include/meta_system.h"

#include "mono_transform_service/include/transform_service.h"
#include "mono_transform_extension/include/allocator_factory.h"
#include "mono_transform_extension/include/transform_component.h"

#include "mono_scene_extension/include/allocator_factory.h"
#include "mono_scene_extension/include/scene_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_scene_extension/include/scene_system.h"

#include "mono_window_service/include/window_service.h"
#include "mono_window_extension/include/allocator_factory.h"
#include "mono_window_extension/include/window_component.h"

#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"
#include "mono_graphics_service/include/graphics_service.h"
#include "mono_graphics_service/include/graphics_command_list.h"
#include "mono_graphics_extension/include/allocator_factory.h"
#include "mono_graphics_extension/include/allocator_factory.h"
#include "mono_graphics_extension/include/window_render_bind_component.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_graphics_extension/include/directional_light_component.h"
#include "mono_graphics_extension/include/ambient_light_component.h"
#include "mono_graphics_extension/include/point_light_component.h"
#include "mono_graphics_extension/include/graphics_system.h"

namespace mono_graphics_extension_test
{

constexpr uint32_t MAX_LIGHT_COUNT = 10;

// A triangle mesh asset handle for testing purposes
class TriangleMeshAssetHandle : public asset_loader::AssetHandle<TriangleMeshAssetHandle> {};

constexpr XMFLOAT4 TRIANGLE_BASE_COLOR = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
constexpr XMFLOAT4 TRIANGLE_EMISSION_COLOR = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

// A marble bust mesh asset handle for testing purposes
class MarbleBustMeshAssetHandle : public asset_loader::AssetHandle<MarbleBustMeshAssetHandle> {};

constexpr XMFLOAT4 MARBLE_BUST_BASE_COLOR = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr XMFLOAT4 MARBLE_BUST_EMISSION_COLOR = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

// A marble bust albedo texture asset handle for testing purposes
class MarbleBustAlbedoTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustAlbedoTextureAssetHandle> {};

// A marble bust normal texture asset handle for testing purposes
class MarbleBustNormalTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustNormalTextureAssetHandle> {};

// A marble bust ambient occlusion texture asset handle for testing purposes
class MarbleBustAOTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustAOTextureAssetHandle> {};

// A marble bust roughness texture asset handle for testing purposes
class MarbleBustRoughnessTextureAssetHandle : public asset_loader::AssetHandle<MarbleBustRoughnessTextureAssetHandle> {};

// Paths to marble bust
constexpr const char* MARBLE_BUST_MODEL_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_1k.mfm";
constexpr const char* MARBLE_BUST_ALBEDO_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_diff_1k.png";
constexpr const char* MARBLE_BUST_NORMAL_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_nor_dx_1k.png";
constexpr const char* MARBLE_BUST_AO_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_ao_1k.png";
constexpr const char* MARBLE_BUST_ROUGHNESS_TEXTURE_PATH 
    = "../resources/render_graph_test/marble_bust_01_1k/marble_bust_01_rough_1k.png";

// A floor mesh asset handle for testing purposes
class FloorMeshAssetHandle : public asset_loader::AssetHandle<FloorMeshAssetHandle> {};

// Paths to floor
constexpr const char* FLOOR_MODEL_PATH 
    = "../resources/render_graph_test/floor/floor.mfm";

// A empty texture asset handle for testing purposes
class EmptyTextureAssetHandle : public asset_loader::AssetHandle<EmptyTextureAssetHandle> {};

// Initial scene
class InitialScene : public mono_scene_extension::SceneBase<InitialScene>
{
public:
    InitialScene(
        mono_service::ServiceProxyManager& service_proxy_manager, 
        const ecs::Entity& scene_entity, const ecs::Entity& window_entity) :
        service_proxy_manager_(service_proxy_manager),
        scene_entity_(scene_entity),
        window_entity_(window_entity)
    {
    }

    virtual ~InitialScene() = default;

    // Load scene assets
    virtual bool LoadAssets(ecs::World& world) override
    {
        // Create entity with asset request component to load asset
        {
            asset_request_entity_ = world.CreateEntity();

            std::unique_ptr<mono_asset_extension::AssetRequestComponent::SetupParam> asset_request_param 
                = std::make_unique<mono_asset_extension::AssetRequestComponent::SetupParam>();

            // Register mesh asset loader
            {
                std::unique_ptr<asset_loader::AssetLoader> mesh_loader 
                    = std::make_unique<mono_asset_extension::MeshAssetLoader>();

                asset_request_param->needs_register_loaders.emplace(
                    mono_asset_extension::MeshAssetLoader::ID(), std::move(mesh_loader));
            }

            // Register texture asset loader
            {
                std::unique_ptr<asset_loader::AssetLoader> texture_loader 
                    = std::make_unique<mono_asset_extension::TextureAssetLoader>();

                asset_request_param->needs_register_loaders.emplace(
                    mono_asset_extension::TextureAssetLoader::ID(), std::move(texture_loader));
            }

            // Add triangle mesh asset source
            {
                std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create mesh asset source data
                    mesh_source_data 
                        = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Create geometry data
                std::unique_ptr<geometry::Geometry> geometry = std::make_unique<geometry::Triangle>(
                    XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                    XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
                    XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f));

                // Set geometry data
                mesh_source_data->SetGeometry(std::move(geometry));

                // Create asset description for mesh asset
                std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::TriangleMeshAssetHandle::ID(), 
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
                std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create mesh asset source data
                    mesh_source_data 
                        = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set MFM data path
                mesh_source_data->SetFilePath(MARBLE_BUST_MODEL_PATH);

                // Create asset description for marble bust mesh asset
                std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::MarbleBustMeshAssetHandle::ID(), 
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
                std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create texture asset source data
                    texture_source_data 
                        = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set file path
                texture_source_data->SetFilePath(MARBLE_BUST_ALBEDO_TEXTURE_PATH);

                // Create asset description for marble bust albedo texture asset
                std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::MarbleBustAlbedoTextureAssetHandle::ID(),
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
                std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create texture asset source data
                    texture_source_data 
                        = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set file path
                texture_source_data->SetFilePath(MARBLE_BUST_NORMAL_TEXTURE_PATH);

                // Create asset description for marble bust normal texture asset
                std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::MarbleBustNormalTextureAssetHandle::ID(),
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
                std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create texture asset source data
                    texture_source_data 
                        = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set file path
                texture_source_data->SetFilePath(MARBLE_BUST_AO_TEXTURE_PATH);

                // Create asset description for marble bust AO texture asset
                std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::MarbleBustAOTextureAssetHandle::ID(),
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
                std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create texture asset source data
                    texture_source_data 
                        = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set file path
                texture_source_data->SetFilePath(MARBLE_BUST_ROUGHNESS_TEXTURE_PATH);

                // Create asset description for marble bust roughness texture asset
                std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::MarbleBustRoughnessTextureAssetHandle::ID(),
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
                std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create mesh asset source data
                    mesh_source_data 
                        = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set MFM data path
                mesh_source_data->SetFilePath(FLOOR_MODEL_PATH);

                // Create asset description for floor mesh asset
                std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::FloorMeshAssetHandle::ID(), 
                        mono_asset_extension::MeshAssetLoader::ID());

                // Create asset source
                std::unique_ptr<asset_loader::AssetSource> mesh_asset_source 
                    = std::make_unique<asset_loader::AssetSource>();
                mesh_asset_source->source_data = std::move(mesh_source_data);
                mesh_asset_source->description = std::move(mesh_asset_desc);

                // Add to asset sources in setup parameter
                asset_request_param->needs_load_asset_sources.emplace_back(std::move(mesh_asset_source));
            }

            // Add empty texture asset source
            {
                std::unique_ptr<mono_asset_extension::TextureAssetSourceData> texture_source_data = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get graphics service proxy
                    mono_service::ServiceProxy& graphics_service_proxy 
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    // Create texture asset source data
                    texture_source_data 
                        = std::make_unique<mono_asset_extension::TextureAssetSourceData>(graphics_service_proxy.Clone());
                });

                // Set texture info for empty texture asset
                mono_asset_extension::TextureAssetSourceData::TextureInfo texture_info = {};
                texture_info.width = 1;
                texture_info.height = 1;
                texture_info.format = DXGI_FORMAT_R8G8B8A8_UNORM;
                texture_source_data->SetTextureInfo(texture_info);

                // Create asset description for empty texture asset
                std::unique_ptr<asset_loader::AssetDescription> texture_asset_desc
                    = std::make_unique<asset_loader::AssetDescription>(
                        mono_graphics_extension_test::EmptyTextureAssetHandle::ID(),
                        mono_asset_extension::TextureAssetLoader::ID());

                // Create asset source
                std::unique_ptr<asset_loader::AssetSource> texture_asset_source 
                    = std::make_unique<asset_loader::AssetSource>();
                texture_asset_source->source_data = std::move(texture_source_data);
                texture_asset_source->description = std::move(texture_asset_desc);

                // Add to asset sources in setup parameter
                asset_request_param->needs_load_asset_sources.emplace_back(std::move(texture_asset_source));
            }

            // Add AssetRequestComponent to the entity
            bool result = world.AddComponent<mono_asset_extension::AssetRequestComponent>(
                asset_request_entity_, mono_asset_extension::AssetRequestComponentHandle::ID(), std::move(asset_request_param));
            assert(result);
        }

        return true; // Success
    }

    // Check if scene assets are loaded
    virtual bool IsAssetsLoaded(ecs::World& world) const override
    {
        // Create asset service view
        std::unique_ptr<mono_service::ServiceView> asset_service_view = nullptr;
        service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Get asset service proxy
            mono_service::ServiceProxy& asset_service_proxy 
                = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

            // Create asset service view
            asset_service_view = asset_service_proxy.CreateView();
        });

        // Cast to asset service view
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Check if the triangle mesh asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::TriangleMeshAssetHandle::ID()))
            return false; // Not loaded
        
        // Check if the marble bust mesh asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::MarbleBustMeshAssetHandle::ID()))
            return false; // Not loaded

        // Check if the marble bust albedo texture asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::MarbleBustAlbedoTextureAssetHandle::ID()))
            return false; // Not loaded

        // Check if the marble bust normal texture asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::MarbleBustNormalTextureAssetHandle::ID()))
            return false; // Not loaded

        // Check if the marble bust AO texture asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::MarbleBustAOTextureAssetHandle::ID()))
            return false; // Not loaded

        // Check if the marble bust roughness texture asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::MarbleBustRoughnessTextureAssetHandle::ID()))
            return false; // Not loaded

        // Check if the floor mesh asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::FloorMeshAssetHandle::ID()))
            return false; // Not loaded

        // Check if the empty texture asset is loaded
        if (!asset_service_view_ptr->IsAssetLoaded(mono_graphics_extension_test::EmptyTextureAssetHandle::ID()))
            return false; // Not loaded

        return true; // Always loaded
    }

    // Release scene assets
    virtual bool ReleaseAssets(ecs::World& world) override
    {
        return true; // Success
    }

    // Check if scene assets are released
    virtual bool IsAssetsReleased(ecs::World& world) const override
    {
        return true; // Always released
    }

    // Create scene entities
    virtual bool Create(ecs::World& world) override
    {
        if (!CreateMaterials(world))
            return false; // Failed to create materials

        if (!CreateEntities(world))
            return false; // Failed to create entities

        return true; // Success
    }

    virtual bool Destroy(ecs::World& world) override
    {
        if (!DestroyEntities(world))
            return false; // Failed to destroy entities

        return true; // Success
    }

private:
    // Create scene materials
    bool CreateMaterials(ecs::World& world)
    {
        // Loading is complete, destroy asset request entity
        assert(
            world.CheckEntityExist(asset_request_entity_) && 
            "Asset request entity must exist until assets are loaded!");
        world.DestroyEntity(asset_request_entity_);

        std::unique_ptr<mono_service::ServiceView> asset_service_view = nullptr;
        std::unique_ptr<mono_service::ServiceCommandList> graphics_command_list = nullptr;
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
        service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Clone graphics service proxy
            graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
                
            // Create graphics service command list
            graphics_command_list = graphics_service_proxy->CreateCommandList();

            // Get asset service proxy
            mono_service::ServiceProxy& asset_service_proxy 
                = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

            // Create asset service view
            asset_service_view = asset_service_proxy.CreateView();
        });

        // Cast to asset service view
        mono_asset_service::AssetServiceView* asset_service_view_ptr
            = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
        assert(asset_service_view_ptr != nullptr && "Asset service view is null!");

        // Cast to graphics service command list
        mono_graphics_service::GraphicsCommandList* graphics_command_list_ptr
            = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(graphics_command_list.get());
        assert(graphics_command_list_ptr != nullptr && "Graphics command list is null!");

        {
            // Create marble bust lambert material setup parameter
            std::unique_ptr<render_graph::PhongMaterial::SetupParam> phong_material_param
                = std::make_unique<render_graph::PhongMaterial::SetupParam>();
            phong_material_param->base_color = mono_graphics_extension_test::MARBLE_BUST_BASE_COLOR;
            phong_material_param->albedo_source = render_graph::PhongMaterial::ALBEDO_SOURCE_TEXTURE;
            phong_material_param->normal_source = render_graph::PhongMaterial::NORMAL_SOURCE_TEXTURE;
            phong_material_param->ao_source = render_graph::PhongMaterial::AO_SOURCE_TEXTURE;
            phong_material_param->specular_source = render_graph::PhongMaterial::SPECULAR_SOURCE_NONE;
            phong_material_param->roughness_source = render_graph::PhongMaterial::ROUGHNESS_SOURCE_TEXTURE;
            phong_material_param->roughness_value = 1.0f;
            phong_material_param->metalness_source = render_graph::PhongMaterial::METALNESS_SOURCE_VALUE;
            phong_material_param->metalness_value = 0.0f;
            phong_material_param->emission_source = render_graph::PhongMaterial::EMISSION_SOURCE_COLOR;
            phong_material_param->emission_color = mono_graphics_extension_test::MARBLE_BUST_EMISSION_COLOR;

            // Get empty texture asset
            const asset_loader::Asset& empty_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::EmptyTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);
            assert(empty_texture_asset_ptr != nullptr && "Failed to get empty texture asset!");

            // Get marble bust albedo texture asset
            const asset_loader::Asset& marble_bust_albedo_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::MarbleBustAlbedoTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* marble_bust_albedo_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_albedo_texture_asset);
            assert(marble_bust_albedo_texture_asset_ptr != nullptr && "Failed to get marble bust albedo texture asset!");

            // Get marble bust normal texture asset
            const asset_loader::Asset& marble_bust_normal_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::MarbleBustNormalTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* marble_bust_normal_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_normal_texture_asset);
            assert(marble_bust_normal_texture_asset_ptr != nullptr && "Failed to get marble bust normal texture asset!");

            // Get marble bust AO texture asset
            const asset_loader::Asset& marble_bust_ao_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::MarbleBustAOTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* marble_bust_ao_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_ao_texture_asset);
            assert(marble_bust_ao_texture_asset_ptr != nullptr && "Failed to get marble bust AO texture asset!");

            // Get marble bust roughness texture asset
            const asset_loader::Asset& marble_bust_roughness_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::MarbleBustRoughnessTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* marble_bust_roughness_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&marble_bust_roughness_texture_asset);
            assert(marble_bust_roughness_texture_asset_ptr != nullptr && "Failed to get marble bust roughness texture asset!");

            phong_material_param->albedo_texture_handle = marble_bust_albedo_texture_asset_ptr->GetTextureHandle();
            phong_material_param->normal_texture_handle = marble_bust_normal_texture_asset_ptr->GetTextureHandle();
            phong_material_param->ao_texture_handle = marble_bust_ao_texture_asset_ptr->GetTextureHandle();
            phong_material_param->specular_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
            phong_material_param->roughness_texture_handle = marble_bust_roughness_texture_asset_ptr->GetTextureHandle();
            phong_material_param->metalness_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
            phong_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

            // Create marble bust phong material
            graphics_command_list_ptr->CreateMaterial<render_graph::PhongMaterial>(
                &marble_bust_phong_material_handle_, std::move(phong_material_param));
        }

        {
            // Create floor lambert material setup parameter
            std::unique_ptr<render_graph::LambertMaterial::SetupParam> lambert_material_param
                = std::make_unique<render_graph::LambertMaterial::SetupParam>();
            lambert_material_param->base_color = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
            lambert_material_param->albedo_source = render_graph::LambertMaterial::ALBEDO_SOURCE_BASE_COLOR;
            lambert_material_param->normal_source = render_graph::LambertMaterial::NORMAL_SOURCE_VERTEX;
            lambert_material_param->ao_source = render_graph::LambertMaterial::AO_SOURCE_NONE;
            lambert_material_param->emission_source = render_graph::LambertMaterial::EMISSION_SOURCE_NONE;
            lambert_material_param->emission_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

            // Get empty texture asset
            const asset_loader::Asset& empty_texture_asset
                = asset_service_view_ptr->GetAsset(mono_graphics_extension_test::EmptyTextureAssetHandle::ID());
            const mono_asset_extension::TextureAsset* empty_texture_asset_ptr
                = dynamic_cast<const mono_asset_extension::TextureAsset*>(&empty_texture_asset);
            assert(empty_texture_asset_ptr != nullptr && "Failed to get empty texture asset!");

            lambert_material_param->albedo_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
            lambert_material_param->normal_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
            lambert_material_param->ao_texture_handle = empty_texture_asset_ptr->GetTextureHandle();
            lambert_material_param->emission_texture_handle = empty_texture_asset_ptr->GetTextureHandle();

            // Create floor lambert material
            graphics_command_list_ptr->CreateMaterial<render_graph::LambertMaterial>(
                &floor_lambert_material_handle_, std::move(lambert_material_param));
        }

        // Submit command list
        graphics_service_proxy->SubmitCommandList(std::move(graphics_command_list));

        return true; // Success
    }

    // Create scene entities
    bool CreateEntities(ecs::World& world)
    {
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

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& transform_service_proxy
                        = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

                    bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                        entity, mono_transform_extension::TransformComponentHandle::ID(),
                        std::move(param), transform_service_proxy.Clone());
                    assert(result);
                });
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

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& graphics_service_proxy
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    bool result = world.AddComponent<mono_graphics_extension::CameraComponent>(
                        entity, mono_graphics_extension::CameraComponentHandle::ID(),
                        std::move(param), graphics_service_proxy.Clone());
                    assert(result);
                });
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

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& transform_service_proxy
                        = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

                    bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                        entity, mono_transform_extension::TransformComponentHandle::ID(),
                        std::move(param), transform_service_proxy.Clone());
                    assert(result);
                });
            }

            // Renderable component
            {
                // Create asset service view to get mesh asset handle
                std::unique_ptr<mono_service::ServiceView> service_view = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get asset service proxy
                    mono_service::ServiceProxy& asset_service_proxy
                        = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

                    // Create asset service view
                    service_view = asset_service_proxy.CreateView();
                });
                mono_asset_service::AssetServiceView* asset_service_view
                    = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
                assert(asset_service_view != nullptr && "Asset service view is null!");

                // Get marble bust mesh asset
                const asset_loader::Asset& mesh_asset
                    = asset_service_view->GetAsset(mono_graphics_extension_test::MarbleBustMeshAssetHandle::ID());
                const mono_asset_extension::MeshAsset* mesh_asset_ptr
                    = dynamic_cast<const mono_asset_extension::MeshAsset*>(&mesh_asset);
                assert(mesh_asset_ptr != nullptr && "Failed to get marble bust mesh asset!");

                std::unique_ptr<mono_graphics_extension::RenderableComponent::SetupParam> param
                    = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
                param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
                param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
                param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts(); 
                
                std::vector<const render_graph::MaterialHandle*> material_handles;
                for (size_t i = 0; i < mesh_asset_ptr->GetIndexCounts()->size(); ++i)
                    material_handles.push_back(&marble_bust_phong_material_handle_);
                param->mesh_set_.material_handles = std::move(material_handles);

                param->mesh_set_.mesh_asset_handle = mono_graphics_extension_test::MarbleBustMeshAssetHandle::ID();

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& graphics_service_proxy
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    bool result = world.AddComponent<mono_graphics_extension::RenderableComponent>(
                        entity, mono_graphics_extension::RenderableComponentHandle::ID(),
                        std::move(param), graphics_service_proxy.Clone());
                    assert(result);
                });
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

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& transform_service_proxy
                        = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

                    bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                        entity, mono_transform_extension::TransformComponentHandle::ID(),
                        std::move(param), transform_service_proxy.Clone());
                    assert(result);
                });
            }

            // Renderable component
            {
                // Create asset service view to get mesh asset handle
                std::unique_ptr<mono_service::ServiceView> service_view = nullptr;
                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    // Get asset service proxy
                    mono_service::ServiceProxy& asset_service_proxy
                        = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

                    // Create asset service view
                    service_view = asset_service_proxy.CreateView();
                });
                mono_asset_service::AssetServiceView* asset_service_view
                    = dynamic_cast<mono_asset_service::AssetServiceView*>(service_view.get());
                assert(asset_service_view != nullptr && "Asset service view is null!");

                // Get floor mesh asset
                const asset_loader::Asset& mesh_asset
                    = asset_service_view->GetAsset(mono_graphics_extension_test::FloorMeshAssetHandle::ID());
                const mono_asset_extension::MeshAsset* mesh_asset_ptr
                    = dynamic_cast<const mono_asset_extension::MeshAsset*>(&mesh_asset);
                assert(mesh_asset_ptr != nullptr && "Failed to get floor mesh asset!");

                std::unique_ptr<mono_graphics_extension::RenderableComponent::SetupParam> param
                    = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();
                param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
                param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
                param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();  

                std::vector<const render_graph::MaterialHandle*> material_handles;
                for (size_t i = 0; i < mesh_asset_ptr->GetIndexCounts()->size(); ++i)
                    material_handles.push_back(&floor_lambert_material_handle_);
				param->mesh_set_.material_handles = std::move(material_handles);

                param->mesh_set_.mesh_asset_handle = mono_graphics_extension_test::FloorMeshAssetHandle::ID();

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& graphics_service_proxy
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    bool result = world.AddComponent<mono_graphics_extension::RenderableComponent>(
                        entity, mono_graphics_extension::RenderableComponentHandle::ID(),
                        std::move(param), graphics_service_proxy.Clone());
                    assert(result);
                });
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

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& transform_service_proxy
                        = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

                    bool result = world.AddComponent<mono_transform_extension::TransformComponent>(
                        entity, mono_transform_extension::TransformComponentHandle::ID(),
                        std::move(param), transform_service_proxy.Clone());
                    assert(result);
                });
            }

            // Directional Light component
            {
                std::unique_ptr<mono_graphics_extension::DirectionalLightComponent::SetupParam> param
                    = std::make_unique<mono_graphics_extension::DirectionalLightComponent::SetupParam>();

                service_proxy_manager_.WithLock([&](mono_service::ServiceProxyManager& manager)
                {
                    mono_service::ServiceProxy& graphics_service_proxy
                        = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                    bool result = world.AddComponent<mono_graphics_extension::DirectionalLightComponent>(
                        entity, mono_graphics_extension::DirectionalLightComponentHandle::ID(),
                        std::move(param), graphics_service_proxy.Clone(), mono_graphics_extension::BACK_BUFFER_COUNT);
                    assert(result);
                });
            }
        }

        return true; // Success
    }

    // Destroy scene entities
    bool DestroyEntities(ecs::World& world)
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

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;

    // The scene entity
    ecs::Entity scene_entity_ = ecs::Entity();

    // The window entity
    ecs::Entity window_entity_ = ecs::Entity();

    // The asset request entity
    ecs::Entity asset_request_entity_ = ecs::Entity();

    // The marble bust lambert material handle
    render_graph::MaterialHandle marble_bust_phong_material_handle_ = render_graph::MaterialHandle();

    // The floor lambert material handle
    render_graph::MaterialHandle floor_lambert_material_handle_ = render_graph::MaterialHandle();
};

// Window procedure
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT menu
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

// Create ImGui context
ImGuiContext* CreateImGuiContext(HWND hwnd)
{
    bool result = false;

    // Get main monitor scale
    float main_scale 
        = ImGui_ImplWin32_GetDpiScaleForMonitor(::MonitorFromPoint(POINT{ 0, 0 }, MONITOR_DEFAULTTOPRIMARY));

     // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* context = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();

    // Bake a fixed style scale. 
    // (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.ScaleAllSizes(main_scale);

    // Set initial font scale. 
    // (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    style.FontScaleDpi = main_scale;

    // Setup Platform/Renderer backends
    result = ImGui_ImplWin32_Init(hwnd);
    if (!result)
        return nullptr;

    ImGui_ImplDX12_InitInfo init_info = {};
    init_info.Device = dx12_util::Device::GetInstance().Get();
    init_info.CommandQueue = dx12_util::CommandQueue::GetInstance().Get();
    init_info.NumFramesInFlight = mono_graphics_extension::BACK_BUFFER_COUNT;
    init_info.RTVFormat = render_graph::composition_pass::SWAP_CHAIN_FORMAT;
    init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

    // Get heap manager
    render_graph::HeapManager& heap_manager = render_graph::HeapManager::GetInstance();

    heap_manager.WithUniqueLock([&](render_graph::HeapManager& heap_manager)
    {
        // Get SRV descriptor heap
        dx12_util::DescriptorHeap& srv_heap = heap_manager.GetSrvHeap();

        // Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
        init_info.SrvDescriptorHeap = srv_heap.Get();
        init_info.SrvDescriptorAllocFn = [](
            ImGui_ImplDX12_InitInfo*, 
            D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
            {
                render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                {
                    dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                    srv_heap_allocator.Allocate(*out_cpu_handle, *out_gpu_handle);
                });
            };
        init_info.SrvDescriptorFreeFn = [](
            ImGui_ImplDX12_InitInfo*,
            D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
            {
                render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                {
                    dx12_util::DescriptorHeapAllocator& srv_heap_allocator = heap_manager.GetSrvHeapAllocator();
                    srv_heap_allocator.Free(cpu_handle, gpu_handle);
                });
            };
    });

    result = ImGui_ImplDX12_Init(&init_info);
    if (!result)
        return nullptr;
    
    return context;
}

// Destroy ImGui context
void DestroyImguiContext(ImGuiContext* context)
{
    // Set current ImGui context
    ImGui::SetCurrentContext(context);

    // Shutdown ImGui
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
};

bool RunMessageLoop(bool& running,  std::function<bool()> frame_func)
{
    MSG msg = {};
    while (running)
    {
        while (::PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);

            if (msg.message == WM_QUIT)
                return true;
        }

        if (!running)
            break; // Exit if no longer running

        // Run frame function
        running = frame_func();
    }

    return true;
}

} // namespace mono_graphics_extension_test

TEST(System, DrawTriangle)
{
    // Co-initialize COM library
    HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

    bool result = false;

    /*******************************************************************************************************************
     * Import services
    /******************************************************************************************************************/

    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator
        = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry
        = std::make_unique<mono_service::ServiceRegistry>();

    // Import asset service in to registry
    constexpr mono_service::ServiceThreadAffinityID ASSET_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_asset_service::AssetService::SetupParam asset_service_setup_param;
        result = mono_service::ImportService<mono_asset_service::AssetService, mono_asset_service::AssetServiceHandle>(
            *service_registry, ASSET_SERVICE_THREAD_AFFINITY_ID, asset_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Import transform service in to registry
    constexpr mono_service::ServiceThreadAffinityID TRANSFORM_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_transform_service::TransformService::SetupParam transform_service_setup_param;
        result = mono_service::ImportService<mono_transform_service::TransformService, mono_transform_service::TransformServiceHandle>(
            *service_registry, TRANSFORM_SERVICE_THREAD_AFFINITY_ID, transform_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Import window service in to registry
    constexpr mono_service::ServiceThreadAffinityID WINDOW_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_window_service::WindowService::SetupParam window_service_setup_param;
        result = mono_service::ImportService<mono_window_service::WindowService, mono_window_service::WindowServiceHandle>(
            *service_registry, WINDOW_SERVICE_THREAD_AFFINITY_ID, window_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Import graphics service in to registry
    constexpr mono_service::ServiceThreadAffinityID GRAPHICS_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_graphics_service::GraphicsService::SetupParam graphics_service_setup_param;
        result = mono_service::ImportService<mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
            *service_registry, GRAPHICS_SERVICE_THREAD_AFFINITY_ID, graphics_service_setup_param);
        ASSERT_TRUE(result);
    }

    // Create service proxy registry
    std::unique_ptr<mono_service::ServiceProxyRegistry> service_proxy_registry
        = std::make_unique<mono_service::ServiceProxyRegistry>();

    // Get service proxies for imported services and register them in the proxy registry
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        for (const auto& service_id : registry.GetRegisteredIDs())
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            service_proxy_registry->WithUniqueLock([&](mono_service::ServiceProxyRegistry& proxy_registry)
            {
                // Register service proxy in to proxy registry
                proxy_registry.Register(
                    service_id, service.CreateServiceProxy());
            });
        }
    });

    // Create service proxy manager
    std::unique_ptr<mono_service::ServiceProxyManager> service_proxy_manager
        = std::make_unique<mono_service::ServiceProxyManager>(*service_proxy_registry);

    /*******************************************************************************************************************
     * Create systems
    /******************************************************************************************************************/

    // Create singleton system id generator
    std::unique_ptr<ecs::SystemIDGenerator> system_id_generator
        = std::make_unique<ecs::SystemIDGenerator>();

    // Create system map
    std::unordered_map<ecs::SystemID, std::unique_ptr<ecs::System>> systems;

    service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        // Get asset service proxy
        mono_service::ServiceProxy& asset_service_proxy 
            = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

        // Get transform service proxy
        mono_service::ServiceProxy& transform_service_proxy 
            = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

        // Get graphics service proxy
        mono_service::ServiceProxy& graphics_service_proxy 
            = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

        // Create meta system and add to system map
        std::unique_ptr<ecs::System> meta_system
            = std::make_unique<mono_meta_extension::MetaSystem>();
        systems.emplace(meta_system->GetID(), std::move(meta_system));

        // Create asset request processor system and add to system map
        std::unique_ptr<ecs::System> asset_request_processor_system
            = std::make_unique<mono_asset_extension::AssetRequestProcessorSystem>(asset_service_proxy.Clone());
        systems.emplace(asset_request_processor_system->GetID(), std::move(asset_request_processor_system));

        // Create scene system and add to system map
        std::unique_ptr<ecs::System> scene_system
            = std::make_unique<mono_scene_extension::SceneSystem>();
        systems.emplace(scene_system->GetID(), std::move(scene_system));

        // Create graphics system and add to system map
        std::unique_ptr<ecs::System> graphics_system
            = std::make_unique<mono_graphics_extension::GraphicsSystem>(graphics_service_proxy.Clone());
        systems.emplace(graphics_system->GetID(), std::move(graphics_system));
    });

    /*******************************************************************************************************************
     * Create ECS world
    /******************************************************************************************************************/

    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator
        = std::make_unique<ecs::ComponentIDGenerator>();

    std::unique_ptr<ecs::World> ecs_world = nullptr;
    {
        // Create component descriptor registry
        std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry
            = std::make_unique<ecs::ComponentDescriptorRegistry>();

        // Register MetaComponent descriptor
        constexpr size_t MAX_META_COMPONENT_COUNT = 20;
        ecs::RegisterComponentDescriptor<
            mono_meta_extension::MetaComponent, 
            mono_meta_extension::ComponentAllocatorFactory,
            mono_meta_extension::MetaComponentHandle>(
                *component_descriptor_registry, MAX_META_COMPONENT_COUNT);

        // Register AssetRequestComponent descriptor
        constexpr size_t MAX_ASSET_REQUEST_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_asset_extension::AssetRequestComponent, 
            mono_asset_extension::ComponentAllocatorFactory,
            mono_asset_extension::AssetRequestComponentHandle>(
                *component_descriptor_registry, MAX_ASSET_REQUEST_COMPONENT_COUNT);
        
        // Register TransformComponent descriptor
        constexpr size_t MAX_TRANSFORM_COMPONENT_COUNT = 10;
        ecs::RegisterComponentDescriptor<
            mono_transform_extension::TransformComponent, 
            mono_transform_extension::ComponentAllocatorFactory,
            mono_transform_extension::TransformComponentHandle>(
                *component_descriptor_registry, MAX_TRANSFORM_COMPONENT_COUNT);

        // Register SceneTagComponent descriptor
        constexpr size_t MAX_SCENE_TAG_COMPONENT_COUNT = 20;
        ecs::RegisterComponentDescriptor<
            mono_scene_extension::SceneTagComponent, 
            mono_scene_extension::ComponentAllocatorFactory,
            mono_scene_extension::SceneTagComponentHandle>(
                *component_descriptor_registry, MAX_SCENE_TAG_COMPONENT_COUNT);

        // Register SceneComponent descriptor
        constexpr size_t MAX_SCENE_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_scene_extension::SceneComponent, 
            mono_scene_extension::ComponentAllocatorFactory,
            mono_scene_extension::SceneComponentHandle>(
                *component_descriptor_registry, MAX_SCENE_COMPONENT_COUNT);

        // Register WindowComponent descriptor
        constexpr size_t MAX_WINDOW_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_window_extension::WindowComponent, 
            mono_window_extension::ComponentAllocatorFactory,
            mono_window_extension::WindowComponentHandle>(
                *component_descriptor_registry, MAX_WINDOW_COMPONENT_COUNT);

        // Register WindowRenderBindComponent descriptor
        constexpr size_t MAX_WINDOW_RENDER_BIND_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::WindowRenderBindComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::WindowRenderBindComponentHandle>(
                *component_descriptor_registry, MAX_WINDOW_RENDER_BIND_COMPONENT_COUNT);

        // Register CameraComponent descriptor
        constexpr size_t MAX_CAMERA_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::CameraComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::CameraComponentHandle>(
                *component_descriptor_registry, MAX_CAMERA_COMPONENT_COUNT);

        // Register RenderableComponent descriptor
        constexpr size_t MAX_RENDERABLE_COMPONENT_COUNT = 10;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::RenderableComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::RenderableComponentHandle>(
                *component_descriptor_registry, MAX_RENDERABLE_COMPONENT_COUNT);

        // Register UIComponent descriptor
        constexpr size_t MAX_UI_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::UIComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::UIComponentHandle>(
                *component_descriptor_registry, MAX_UI_COMPONENT_COUNT);

        // Register DirectionalLightComponent descriptor
        constexpr size_t MAX_DIRECTIONAL_LIGHT_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::DirectionalLightComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::DirectionalLightComponentHandle>(
                *component_descriptor_registry, MAX_DIRECTIONAL_LIGHT_COMPONENT_COUNT);

        // Register AmbientLightComponent descriptor
        constexpr size_t MAX_AMBIENT_LIGHT_COMPONENT_COUNT = 5;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::AmbientLightComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::AmbientLightComponentHandle>(
                *component_descriptor_registry, MAX_AMBIENT_LIGHT_COMPONENT_COUNT);

        // Register PointLightComponent descriptor
        constexpr size_t MAX_POINT_LIGHT_COMPONENT_COUNT = 10;
        ecs::RegisterComponentDescriptor<
            mono_graphics_extension::PointLightComponent, 
            mono_graphics_extension::ComponentAllocatorFactory,
            mono_graphics_extension::PointLightComponentHandle>(
                *component_descriptor_registry, MAX_POINT_LIGHT_COMPONENT_COUNT);

        // Create the ecs world
        ecs_world = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
    }

    /*******************************************************************************************************************
     * Create entity with components
    /******************************************************************************************************************/

    // Window
    ecs::Entity window_entity = ecs_world->CreateEntity();
    {
        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Window";

            result = ecs_world->AddComponent<mono_meta_extension::MetaComponent>(
                window_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            ASSERT_TRUE(result);
        }

        // Scene tag component
        {
            std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
            param->scene_id = mono_graphics_extension_test::InitialScene::ID();

            result = ecs_world->AddComponent<mono_scene_extension::SceneTagComponent>(
                window_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
            ASSERT_TRUE(result);
        }

        // Window component
        {
            std::unique_ptr<mono_window_extension::WindowComponent::SetupParam> param
                = std::make_unique<mono_window_extension::WindowComponent::SetupParam>();
            param->title = L"Test Window";
            param->class_name = L"TestWindowClass";
            param->window_proc = mono_graphics_extension_test::WndProc;
            param->width = 1200;
            param->height = 800;

            service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
            {
                // Get window service proxy
                mono_service::ServiceProxy& window_service_proxy 
                    = manager.GetServiceProxy(mono_window_service::WindowServiceHandle::ID());

                result = ecs_world->AddComponent<mono_window_extension::WindowComponent>(
                    window_entity, mono_window_extension::WindowComponentHandle::ID(), 
                    std::move(param), window_service_proxy.Clone());
                ASSERT_TRUE(result);
            });
        }

        // Window render bind component
        {
            std::unique_ptr<mono_graphics_extension::WindowRenderBindComponent::SetupParam> param
                = std::make_unique<mono_graphics_extension::WindowRenderBindComponent::SetupParam>();
            param->imgui_create_func = mono_graphics_extension_test::CreateImGuiContext;
            param->imgui_destroy_func = mono_graphics_extension_test::DestroyImguiContext;

            service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
            {
                // Get graphics service proxy
                mono_service::ServiceProxy& graphics_service_proxy 
                    = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                result = ecs_world->AddComponent<mono_graphics_extension::WindowRenderBindComponent>(
                    window_entity, mono_graphics_extension::WindowRenderBindComponentHandle::ID(), 
                    std::move(param), graphics_service_proxy.Clone());
                ASSERT_TRUE(result);
            });
        }
    }

    // Initial scene
    {
        ecs::Entity entity = ecs_world->CreateEntity();

        // Meta component
        {
            std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
                = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();
            param->name = "Initial Scene";

            result = ecs_world->AddComponent<mono_meta_extension::MetaComponent>(
                entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
            ASSERT_TRUE(result);
        }

        // Scene component
        {
            std::unique_ptr<mono_scene_extension::SceneComponent::SetupParam> param
                = std::make_unique<mono_scene_extension::SceneComponent::SetupParam>();

            // Set required window component IDs
            param->required_window_component_ids = { 
                mono_window_extension::WindowComponentHandle::ID(), 
                mono_graphics_extension::WindowRenderBindComponentHandle::ID()};

            // Set required camera component IDs
            param->required_camera_component_ids = { 
                mono_graphics_extension::CameraComponentHandle::ID() };
            
            // Set required renderable component IDs
            param->required_renderable_component_ids = { 
                mono_graphics_extension::RenderableComponentHandle::ID() };

            // Set required UI component IDs
            param->required_ui_component_ids = {
                mono_graphics_extension::UIComponentHandle::ID() };

            // Set light component IDs
            param->light_component_ids = {
                mono_graphics_extension::DirectionalLightComponentHandle::ID(),
                mono_graphics_extension::AmbientLightComponentHandle::ID(),
                mono_graphics_extension::PointLightComponentHandle::ID() };

            // Set max light count
            param->max_light_count = mono_graphics_extension_test::MAX_LIGHT_COUNT;

            // Set initial state to NeedsLoad
            param->initial_state = mono_scene_extension::SceneState::NeedsLoad;

            // Create initial scene and add to scene map
            std::unique_ptr<mono_scene_extension::Scene> scene
                = std::make_unique<mono_graphics_extension_test::InitialScene>(
                    *service_proxy_manager, entity, window_entity);

            result = ecs_world->AddComponent<mono_scene_extension::SceneComponent>(
                entity, mono_scene_extension::SceneComponentHandle::ID(), std::move(param), 
                mono_graphics_extension_test::InitialScene::ID(), std::move(scene));
            ASSERT_TRUE(result);
        }
    }

    /*******************************************************************************************************************
     * Create system and service execution order
    /******************************************************************************************************************/

    // // Create system execution order
    std::vector<ecs::SystemID> system_execution_order ={
        mono_meta_extension::MetaSystemHandle::ID(),
        mono_asset_extension::AssetRequestProcessorSystemHandle::ID(),
        mono_scene_extension::SceneSystemHandle::ID(),
        mono_graphics_extension::GraphicsSystemHandle::ID()};

    // Create service execution order
    std::vector<mono_service::ServiceHandleID> service_execution_order = {
        mono_asset_service::AssetServiceHandle::ID(),
        mono_transform_service::TransformServiceHandle::ID(),
        mono_window_service::WindowServiceHandle::ID(),
        mono_graphics_service::GraphicsServiceHandle::ID()};

    /*******************************************************************************************************************
     * Update loop
    /******************************************************************************************************************/

    bool is_runnning = true;
    mono_graphics_extension_test::RunMessageLoop(is_runnning, [&]() -> bool
    {
        /***************************************************************************************************************
         * Update systems
        /**************************************************************************************************************/

        for (const auto& system_id : system_execution_order)
        {
            // Get system
            ecs::System& system = *systems.at(system_id);

            // Pre-update
            if (!system.PreUpdate(*ecs_world))
                return false;

            // Update
            if (!system.Update(*ecs_world))
                return false;

            // Post-update
            if (!system.PostUpdate(*ecs_world))
                return false;
        }

        bool service_update_result = true;
        service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
        {
            for (const auto& service_id : service_execution_order)
            {
                // Get service
                mono_service::Service& service = registry.Get(service_id);

                // Pre-update service
                service_update_result = service.PreUpdate();
                if (!service_update_result)
                    return;

                // Update service
                service_update_result = service.Update();
                if (!service_update_result)
                    return;

                // Post-update service
                service_update_result = service.PostUpdate();
                if (!service_update_result)
                    return;
            }
        });

        return service_update_result;
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    ecs_world.reset();
    systems.clear();
    service_proxy_manager.reset();
    service_proxy_registry.reset();
    service_registry.reset();

    // Co-uninitialize COM library
    CoUninitialize();
}