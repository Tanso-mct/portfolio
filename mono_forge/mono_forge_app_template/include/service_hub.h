#pragma once

#include "mono_service/include/service_importer.h"
#include "mono_service/include/thread_affinity.h"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_forge_app_template
{

// Define a service thread affinity handle for main thread
class MainThreadAffinityHandle : public mono_service::ServiceThreadAffinityHandle<MainThreadAffinityHandle> {};

// Define a service thread affinity handle for asset service
class AssetThreadAffinityHandle : public mono_service::ServiceThreadAffinityHandle<AssetThreadAffinityHandle> {};

// Define a service thread affinity handle for transform service
class TransformThreadAffinityHandle : public mono_service::ServiceThreadAffinityHandle<TransformThreadAffinityHandle> {};

// Define a service thread affinity handle for graphics service
class GraphicsThreadAffinityHandle : public mono_service::ServiceThreadAffinityHandle<GraphicsThreadAffinityHandle> {};

// Define a service thread affinity handle for window service
class EntityArchiveThreadAffinityHandle : public mono_service::ServiceThreadAffinityHandle<EntityArchiveThreadAffinityHandle> {};

class ServiceHub
{
public:
    ServiceHub();
    ~ServiceHub();

    // Import a service into the service hub
    template <typename ServiceType, typename HandleType>
    bool Import(
        mono_service::ServiceThreadAffinityID thread_affinity_id, mono_service::Service::SetupParam& setup_param)
    {
        if (!mono_service::ImportService<ServiceType, HandleType>(*service_registry, thread_affinity_id, setup_param))
            return false; // Import failed

        service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
        {
            // Get service
            mono_service::Service& service = registry.Get(HandleType::ID());

            // Register service proxy
            service_proxy_registry->WithUniqueLock([&](mono_service::ServiceProxyRegistry& proxy_registry)
            {
                proxy_registry.Register(HandleType::ID(), service.CreateServiceProxy());
            });
        });

        return true; // Success
    }

    // Update all services in the given order
    bool Update();

    // Get the service proxy manager
    mono_service::ServiceProxyManager& GetServiceProxyManager();

    // Set the execution order of services
    void SetUpdateOrder(std::vector<mono_service::ServiceHandleID> order);

private:
    // The singleton service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator = nullptr;

    // The service registry
    std::unique_ptr<mono_service::ServiceRegistry> service_registry = nullptr;

    // The service proxy registry
    std::unique_ptr<mono_service::ServiceProxyRegistry> service_proxy_registry = nullptr;

    // The service proxy manager
    std::unique_ptr<mono_service::ServiceProxyManager> service_proxy_manager = nullptr;

    // The service execution order
    std::vector<mono_service::ServiceHandleID> service_update_order;

    std::unique_ptr<mono_service::ServiceThreadAffinityIDGenerator> service_thread_affinity_id_generator = nullptr;
};

} // namespace mono_forge_app_template