#include "mono_window_extension_test/pch.h"

#include "ecs/include/world.h"
#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_window_service/include/window_service.h"
#include "mono_window_extension/include/allocator_factory.h"
#include "mono_window_extension/include/window_component.h"

namespace mono_window_extension_test
{

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

} // namespace mono_window_extension_test

TEST(WindowComponent, Use)
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

    // Import window service in to registry
    constexpr mono_service::ServiceThreadAffinityID WINDOW_SERVICE_THREAD_AFFINITY_ID = 0;
    {
        mono_window_service::WindowService::SetupParam window_service_setup_param;
        result = mono_service::ImportService<mono_window_service::WindowService, mono_window_service::WindowServiceHandle>(
            *service_registry, WINDOW_SERVICE_THREAD_AFFINITY_ID, window_service_setup_param);
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

    // Create singleton component id generator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator
        = std::make_unique<ecs::ComponentIDGenerator>();

    std::unique_ptr<ecs::World> ecs_world = nullptr;
    {
        // Create component descriptor registry
        std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry
            = std::make_unique<ecs::ComponentDescriptorRegistry>();

        // Register WindowComponent descriptor
        constexpr size_t WINDOW_COMPONENT_MAX_COUNT = 100;
        {
            std::unique_ptr<ecs::ComponentDescriptor> window_component_desc
                = std::make_unique<ecs::ComponentDescriptor>(
                    sizeof(mono_window_extension::WindowComponent),
                    WINDOW_COMPONENT_MAX_COUNT,
                    std::make_unique<mono_window_extension::ComponentAllocatorFactory>());

            component_descriptor_registry->WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
            {
                // Register the descriptor with component ID
                registry.Register(
                    mono_window_extension::WindowComponentHandle::ID(), std::move(window_component_desc));
            });
        }

        // Create the ecs world
        ecs_world = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
    }

    /*******************************************************************************************************************
     * Create entity and add WindowComponent
    /******************************************************************************************************************/

    {
        // Create entity
        ecs::Entity entity = ecs_world->CreateEntity();

        // Create setup parameters for WindowComponent
        std::unique_ptr<mono_window_extension::WindowComponent::SetupParam> window_component_param
            = std::make_unique<mono_window_extension::WindowComponent::SetupParam>();
        window_component_param->title = L"TestWindow";
        window_component_param->class_name = L"TestWindowClass";
        window_component_param->window_proc = mono_window_extension_test::WndProc;
        window_component_param->width = 500;
        window_component_param->height = 500;

        // Add WindowComponent to entity
        service_proxy_manager->WithLock([&](mono_service::ServiceProxyManager& manager)
        {
            // Get window service proxy
            mono_service::ServiceProxy& window_service_proxy
                = manager.GetServiceProxy(mono_window_service::WindowServiceHandle::ID());

            result = ecs_world->AddComponent<mono_window_extension::WindowComponent>(
                entity, mono_window_extension::WindowComponentHandle::ID(),
                std::move(window_component_param), window_service_proxy.Clone());
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
            mono_window_service::WindowServiceHandle::ID()};

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
     * Show window from WindowComponent
    /******************************************************************************************************************/

    uint32_t window_component_count = 0;
    for (const ecs::Entity& entity : ecs_world->View(mono_window_extension::WindowComponentHandle::ID())())
    {
        mono_window_extension::WindowComponent* window_component
            = ecs_world->GetComponent<mono_window_extension::WindowComponent>(
                entity, mono_window_extension::WindowComponentHandle::ID());
        ASSERT_NE(window_component, nullptr);

        // Get native window handle
        HWND hwnd = window_component->GetHwnd();
        ASSERT_NE(hwnd, nullptr);
        ASSERT_TRUE(::IsWindow(hwnd) != 0);

        // Show the window
        window_component->Show();

        ++window_component_count;
    }

    ASSERT_EQ(window_component_count, 1);

    /*******************************************************************************************************************
     * Update services after showing window
    /******************************************************************************************************************/

    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        // Create service execution order
        std::vector<mono_service::ServiceHandleID> service_execution_order = {
            mono_window_service::WindowServiceHandle::ID()};

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
     * Destroy entity
    /******************************************************************************************************************/

    {
        // Collect entities to destroy
        std::vector<ecs::Entity> entities_to_destroy;
        for (const ecs::Entity& entity : ecs_world->View(mono_window_extension::WindowComponentHandle::ID())())
        {
            entities_to_destroy.push_back(entity);
        }
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
            mono_window_service::WindowServiceHandle::ID()};

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