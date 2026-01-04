#include "mono_transform_extension_test/pch.h"

using namespace DirectX;

#include "ecs/include/world.h"
#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_transform_service/include/transform_service.h"
#include "mono_transform_extension/include/allocator_factory.h"
#include "mono_transform_extension/include/transform_component.h"

TEST(TransformComponent, Use)
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

    // Import transform service in to registry
    constexpr mono_service::ServiceThreadAffinityID TRANSFORM_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_transform_service::TransformService::SetupParam transform_service_setup_param;
        result = mono_service::ImportService<mono_transform_service::TransformService, mono_transform_service::TransformServiceHandle>(
            *service_registry, TRANSFORM_SERVICE_THREAD_AFFINITY_ID, transform_service_setup_param);
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

        // Register TransformComponent
        constexpr size_t MAX_TRANSFORM_COMPONENT_COUNT = 10;
        {
            std::unique_ptr<ecs::ComponentDescriptor> transform_component_desc
                = std::make_unique<ecs::ComponentDescriptor>(
                    sizeof(mono_transform_extension::TransformComponent),
                    MAX_TRANSFORM_COMPONENT_COUNT,
                    std::make_unique<mono_transform_extension::ComponentAllocatorFactory>());

            component_descriptor_registry->WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
            {
                // Register the descriptor with component ID
                registry.Register(
                    mono_transform_extension::TransformComponentHandle::ID(), std::move(transform_component_desc));
            });
        }

        // Create the ecs world
        ecs_world = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
    }

    /*******************************************************************************************************************
     * Create entity with TransformComponent
    /******************************************************************************************************************/

    XMFLOAT3 test_position = XMFLOAT3(1.0f, 2.0f, 3.0f);
    XMFLOAT3 test_rotation = XMFLOAT3(45.0f, 0.0f, 0.0f);
    XMFLOAT3 test_scale = XMFLOAT3(2.0f, 2.0f, 2.0f);
    {
        // Create an entity
        ecs::Entity entity = ecs_world->CreateEntity();

        // Create setup parameters for TransformComponent
        std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> transform_component_param
            = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();
        transform_component_param->position = test_position;
        transform_component_param->rotation = test_rotation;
        transform_component_param->scale = test_scale;

        // Add TransformComponent to the entity
        service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Get transform service proxy
            mono_service::ServiceProxy& transform_service_proxy
                = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID());

            result = ecs_world->AddComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID(),
                std::move(transform_component_param), transform_service_proxy.Clone());
        });
        ASSERT_TRUE(result);
    }

    /*******************************************************************************************************************
     * Update services
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Create service execution order
        std::vector<mono_service::ServiceHandleID> service_execution_order = {
            mono_transform_service::TransformServiceHandle::ID()};

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Post-update
            result = service.PostUpdate();
            ASSERT_TRUE(result);
        }
    });

    /*******************************************************************************************************************
     * Get TransformComponent and verify its data
    /******************************************************************************************************************/

    uint32_t transform_component_count = 0;
    for (const ecs::Entity& entity : ecs_world->View(mono_transform_extension::TransformComponentHandle::ID())())
    {
        mono_transform_extension::TransformComponent* transform_component
            = ecs_world->GetComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID());
        ASSERT_NE(transform_component, nullptr);

        // Verify position
        XMFLOAT3 position = transform_component->GetWorldPosition();
        EXPECT_FLOAT_EQ(position.x, test_position.x);
        EXPECT_FLOAT_EQ(position.y, test_position.y);
        EXPECT_FLOAT_EQ(position.z, test_position.z);

        // Verify rotation
        XMFLOAT3 rotation = transform_component->GetWorldEulerAngles();
        EXPECT_FLOAT_EQ(rotation.x, test_rotation.x);
        EXPECT_FLOAT_EQ(rotation.y, test_rotation.y);
        EXPECT_FLOAT_EQ(rotation.z, test_rotation.z);

        // Verify scale
        XMFLOAT3 scale = transform_component->GetWorldScale();
        EXPECT_FLOAT_EQ(scale.x, test_scale.x);
        EXPECT_FLOAT_EQ(scale.y, test_scale.y);
        EXPECT_FLOAT_EQ(scale.z, test_scale.z);

        // Increment component count
        ++transform_component_count;
    }

    EXPECT_EQ(transform_component_count, 1);

    /*******************************************************************************************************************
     * Set new Transform
    /******************************************************************************************************************/

    XMFLOAT3 new_position = XMFLOAT3(4.0f, 5.0f, 6.0f);
    XMFLOAT3 new_rotation = XMFLOAT3(0.0f, 90.0f, 0.0f);
    XMFLOAT3 new_scale = XMFLOAT3(3.0f, 3.0f, 3.0f);
    for (const ecs::Entity& entity : ecs_world->View(mono_transform_extension::TransformComponentHandle::ID())())
    {
        mono_transform_extension::TransformComponent* transform_component
            = ecs_world->GetComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID());
        ASSERT_NE(transform_component, nullptr);

        // Set new position
        transform_component->SetWorldPosition(new_position);

        // Set new rotation
        transform_component->SetWorldRotation(new_rotation);

        // Set new scale
        transform_component->SetWorldScale(new_scale);
    }

    /*******************************************************************************************************************
     * Update services after entity destruction
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Create service execution order
        std::vector<mono_service::ServiceHandleID> service_execution_order = {
            mono_transform_service::TransformServiceHandle::ID()};

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Post-update
            result = service.PostUpdate();
            ASSERT_TRUE(result);
        }
    });

    /*******************************************************************************************************************
     * Verify updated TransformComponent data
    /******************************************************************************************************************/

    transform_component_count = 0;
    for (const ecs::Entity& entity : ecs_world->View(mono_transform_extension::TransformComponentHandle::ID())())
    {
        mono_transform_extension::TransformComponent* transform_component
            = ecs_world->GetComponent<mono_transform_extension::TransformComponent>(
                entity, mono_transform_extension::TransformComponentHandle::ID());
        ASSERT_NE(transform_component, nullptr);

        // Verify position
        XMFLOAT3 position = transform_component->GetWorldPosition();
        EXPECT_FLOAT_EQ(position.x, new_position.x);
        EXPECT_FLOAT_EQ(position.y, new_position.y);
        EXPECT_FLOAT_EQ(position.z, new_position.z);

        // Verify rotation
        XMFLOAT3 rotation = transform_component->GetWorldEulerAngles();
        EXPECT_FLOAT_EQ(rotation.x, new_rotation.x);
        EXPECT_FLOAT_EQ(rotation.y, new_rotation.y);
        EXPECT_FLOAT_EQ(rotation.z, new_rotation.z);

        // Verify scale
        XMFLOAT3 scale = transform_component->GetWorldScale();
        EXPECT_FLOAT_EQ(scale.x, new_scale.x);
        EXPECT_FLOAT_EQ(scale.y, new_scale.y);
        EXPECT_FLOAT_EQ(scale.z, new_scale.z);

        // Increment component count
        ++transform_component_count;
    }

    /*******************************************************************************************************************
     * Destroy entity
    /******************************************************************************************************************/

    {
        // Get all entities with TransformComponent
        std::vector<ecs::Entity> entities_to_destroy;
        for (const ecs::Entity& entity : ecs_world->View(mono_transform_extension::TransformComponentHandle::ID())())
            entities_to_destroy.push_back(entity);

        EXPECT_EQ(entities_to_destroy.size(), 1);

        // Destroy entities
        for (const auto& entity : entities_to_destroy)
        {
            result = ecs_world->DestroyEntity(entity);
            ASSERT_TRUE(result);
        }
    }

    /*******************************************************************************************************************
     * Update services after entity destruction
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Create service execution order
        std::vector<mono_service::ServiceHandleID> service_execution_order = {
            mono_transform_service::TransformServiceHandle::ID()};

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Pre-update
            result = service.PreUpdate();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Update
            result = service.Update();
            ASSERT_TRUE(result);
        }

        for (const auto& service_id : service_execution_order)
        {
            // Get service
            mono_service::Service& service = registry.Get(service_id);

            // Post-update
            result = service.PostUpdate();
            ASSERT_TRUE(result);
        }
    });

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    ecs_world.reset();
    service_proxy_registry.reset();
    service_registry.reset();
}