#include "mono_forge/src/pch.h"
#include "mono_forge/include/service_hub.h"

namespace mono_forge
{

// bool(Service&)
using ServiceUpdateFunc = std::function<bool(mono_service::Service&)>;

// Helper function to perform parallel service updates
bool ParallelServiceUpdate(
    std::vector<mono_service::ServiceHandleID>& service_update_order,
    mono_service::ServiceRegistry& registry, ServiceUpdateFunc update_func)
{
    // Map to hold futures for service updates
    std::unordered_map<mono_service::ServiceThreadAffinityID, std::future<bool>> service_update_futures;
    bool service_update_result = true;

    for (const auto& service_id : service_update_order)
    {
        // Get service
        mono_service::Service& service = registry.Get(service_id);

        // Get thread affinity ID
        mono_service::ServiceThreadAffinityID thread_affinity_id = service.GetThreadAffinityID();

        if (thread_affinity_id == MainThreadAffinityHandle::ID())
        {
            // Main thread affinity, execute directly
            if (!update_func(service))
                service_update_result = false;
            continue; // Skip to next service
        }

        if (service_update_futures.find(thread_affinity_id) == service_update_futures.end())
        {
            // No existing future for this thread affinity, create one
            service_update_futures[thread_affinity_id] = std::async(std::launch::async, [&service, &update_func]() -> bool
            {
                return update_func(service);
            });
        }
        else // Existing future for this thread affinity
        {
            // Get previous future
            std::future<bool>& previous_future = service_update_futures[thread_affinity_id];

            // Wait for previous update to complete
            if (!previous_future.get())
                service_update_result = false;

            // Create new future for current update
            service_update_futures[thread_affinity_id] = std::async(std::launch::async, [&service, &update_func]() -> bool
            {
                return update_func(service);
            });
        }
    }

    // Iterate through all futures
    for (auto& [thread_affinity_id, future] : service_update_futures)
    {
        // Wait for update to complete
        if (!future.get())
            service_update_result = false;
    }

    return service_update_result;
}

ServiceHub::ServiceHub()
{
    // Create service id generator
    service_id_generator = std::make_unique<mono_service::ServiceIDGenerator>();

    // Create service registry
    service_registry = std::make_unique<mono_service::ServiceRegistry>();

    // Create service proxy registry
    service_proxy_registry = std::make_unique<mono_service::ServiceProxyRegistry>();

    // Create service proxy manager
    service_proxy_manager = std::make_unique<mono_service::ServiceProxyManager>(*service_proxy_registry);

    // Create service thread affinity ID generator
    service_thread_affinity_id_generator = std::make_unique<mono_service::ServiceThreadAffinityIDGenerator>();
}

ServiceHub::~ServiceHub()
{
    // Cleanup
    service_proxy_manager.reset();
    service_proxy_registry.reset();
    service_registry.reset();
    service_id_generator.reset();
}

bool ServiceHub::Update()
{
    bool service_update_result = true;
    service_registry->WithUniqueLock([&](mono_service::ServiceRegistry& registry)
    {
        ParallelServiceUpdate(
            service_update_order, registry,
            [&](mono_service::Service& service) -> bool
            {
                // Pre-update
                return service.PreUpdate();
            });

        ParallelServiceUpdate(
            service_update_order, registry,
            [&](mono_service::Service& service) -> bool
            {
                // Update
                return service.Update();
            });

        ParallelServiceUpdate(
            service_update_order, registry,
            [&](mono_service::Service& service) -> bool
            {
                // Post-update
                return service.PostUpdate();
            });
    });

    return service_update_result;
}

mono_service::ServiceProxyManager& ServiceHub::GetServiceProxyManager()
{
    return *service_proxy_manager;
}

void ServiceHub::SetUpdateOrder(std::vector<mono_service::ServiceHandleID> order)
{
    service_update_order = std::move(order);
}

} // namespace mono_forge