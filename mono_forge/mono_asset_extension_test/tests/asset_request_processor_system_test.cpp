#include "mono_asset_extension_test/pch.h"

using namespace DirectX;

#include "utility_header/file_loader.h"

#include "ecs/include/world.h"
#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/allocator_factory.h"
#include "mono_asset_extension/include/asset_request_component.h"
#include "mono_asset_extension/include/asset_request_processor_system.h"
#include "mono_asset_extension/include/mesh_asset.h"
#include "geometry/include/triangle.h"
#include "mono_forge_model/include/mfm.h"

#include "mono_graphics_service/include/graphics_service.h"

namespace mono_asset_extension_test
{

constexpr const char* TEST_MFM_FILE_PATH = "../resources/mono_forge_model/cube.mfm";

// A test asset handle for testing purposes
class TestAssetHandle : public asset_loader::AssetHandle<TestAssetHandle> {};

// A test model asset handle for testing purposes
class TestModelAssetHandle : public asset_loader::AssetHandle<TestModelAssetHandle> {};

} // namespace mono_asset_extension_test

TEST(Asset, RequestProcessorSystem)
{
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
    }

    // Import graphics service in to registry
    constexpr mono_service::ServiceThreadAffinityID GRAPHICS_SERVICE_THREAD_AFFINITY_ID = 1;
    {
        mono_graphics_service::GraphicsService::SetupParam graphics_service_setup_param;
        result = mono_service::ImportService<mono_graphics_service::GraphicsService, mono_graphics_service::GraphicsServiceHandle>(
            *service_registry, GRAPHICS_SERVICE_THREAD_AFFINITY_ID, graphics_service_setup_param);
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
     * Create asset request processor system
    /******************************************************************************************************************/

    std::unique_ptr<mono_asset_extension::AssetRequestProcessorSystem> asset_request_processor_system = nullptr;
    service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        // Get asset service proxy
        mono_service::ServiceProxy& asset_service_proxy 
            = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

        // Create asset request processor system
        asset_request_processor_system
            = std::make_unique<mono_asset_extension::AssetRequestProcessorSystem>(asset_service_proxy.Clone());
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

        // Register AssetRequestComponent
        constexpr size_t MAX_ASSET_REQUEST_COMPONENT_COUNT = 10;
        {
            std::unique_ptr<ecs::ComponentDescriptor> asset_request_desc 
                = std::make_unique<ecs::ComponentDescriptor>(
                    sizeof(mono_asset_extension::AssetRequestComponent),
                    MAX_ASSET_REQUEST_COMPONENT_COUNT, 
                    std::make_unique<mono_asset_extension::ComponentAllocatorFactory>());

            component_descriptor_registry->WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
            {
                // Register the descriptor with component ID
                registry.Register(
                    mono_asset_extension::AssetRequestComponentHandle::ID(), std::move(asset_request_desc));
            });
        }

        // Create the ecs world
        ecs_world  = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
    }

    /*******************************************************************************************************************
     * Create entity with AssetRequestComponent
    /******************************************************************************************************************/

    {
        // Create an entity
        ecs::Entity entity = ecs_world->CreateEntity();

        // Create setup parameters for AssetRequestComponent
        std::unique_ptr<mono_asset_extension::AssetRequestComponent::SetupParam> asset_request_param 
            = std::make_unique<mono_asset_extension::AssetRequestComponent::SetupParam>();

        // Register mesh asset loader
        {
            std::unique_ptr<asset_loader::AssetLoader> mesh_loader 
                = std::make_unique<mono_asset_extension::MeshAssetLoader>();

            asset_request_param->needs_register_loaders.emplace(
                mono_asset_extension::MeshAssetLoader::ID(), std::move(mesh_loader));
        }

        // Add mesh asset source
        {
            std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data = nullptr;
            service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
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
                    mono_asset_extension_test::TestAssetHandle::ID(), mono_asset_extension::MeshAssetLoader::ID());

            // Create asset source
            std::unique_ptr<asset_loader::AssetSource> mesh_asset_source 
                = std::make_unique<asset_loader::AssetSource>();
            mesh_asset_source->source_data = std::move(mesh_source_data);
            mesh_asset_source->description = std::move(mesh_asset_desc);

            // Add to asset sources in setup parameter
            asset_request_param->needs_load_asset_sources.emplace_back(std::move(mesh_asset_source));
        }

        // Add test model asset source
        {
            std::unique_ptr<mono_asset_extension::MeshAssetSourceData> mesh_source_data = nullptr;
            service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
            {
                // Get graphics service proxy
                mono_service::ServiceProxy& graphics_service_proxy 
                    = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID());

                // Create mesh asset source data
                mesh_source_data 
                    = std::make_unique<mono_asset_extension::MeshAssetSourceData>(graphics_service_proxy.Clone());
            });

            // Set file path for test MFM file
            mesh_source_data->SetFilePath(mono_asset_extension_test::TEST_MFM_FILE_PATH);

            // Create asset description for mesh asset
            std::unique_ptr<asset_loader::AssetDescription> mesh_asset_desc
                = std::make_unique<asset_loader::AssetDescription>(
                    mono_asset_extension_test::TestModelAssetHandle::ID(),
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
        result = ecs_world->AddComponent<mono_asset_extension::AssetRequestComponent>(
            entity, mono_asset_extension::AssetRequestComponentHandle::ID(), std::move(asset_request_param));
        ASSERT_TRUE(result);
    }

    /*******************************************************************************************************************
     * Update asset request processor system
    /******************************************************************************************************************/

    // Pre-update
    result = asset_request_processor_system->PreUpdate(*ecs_world);
    ASSERT_TRUE(result);

    // Update
    result = asset_request_processor_system->Update(*ecs_world);
    ASSERT_TRUE(result);

    // Post-update
    result = asset_request_processor_system->PostUpdate(*ecs_world);
    ASSERT_TRUE(result);

    /*******************************************************************************************************************
     * Update services
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        std::future<void> asset_service_future = std::async([&]()
        {
            mono_service::Service& service = registry.Get(mono_asset_service::AssetServiceHandle::ID());

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);

            // Post-update
            result = service.PostUpdate();
        });

        while (true)
        {
            mono_service::Service& service = registry.Get(mono_graphics_service::GraphicsServiceHandle::ID());

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);

            // Post-update
            result = service.PostUpdate();
            ASSERT_TRUE(result);

            bool all_assets_loaded = false;
            service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
            {
                // Get asset service proxy
                mono_service::ServiceProxy& asset_service_proxy 
                    = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID());

                // Create asset service view
                std::unique_ptr<mono_service::ServiceView> asset_service_view 
                    = asset_service_proxy.CreateView();
                mono_asset_service::AssetServiceView* asset_view 
                    = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view.get());
                ASSERT_NE(asset_view, nullptr);

                // Check if assets are loaded
                bool is_asset_loaded = asset_view->IsAssetLoaded(mono_asset_extension_test::TestAssetHandle::ID());
                bool is_model_loaded = asset_view->IsAssetLoaded(mono_asset_extension_test::TestModelAssetHandle::ID());

                if (is_asset_loaded && is_model_loaded)
                    all_assets_loaded = true;
            });

            // Break the loop if all assets are loaded
            if (all_assets_loaded)
                break;
        }

        asset_service_future.get();
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    asset_request_processor_system.reset();
    ecs_world.reset();
    service_proxy_registry.reset();
    service_registry.reset();
}