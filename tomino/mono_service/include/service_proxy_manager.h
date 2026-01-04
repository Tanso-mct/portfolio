#pragma once

#include <unordered_map>

#include "utility_header/registry.h"

#include "mono_service/include/dll_config.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_registry.h"

namespace mono_service
{

// The registry type for service proxies
using ServiceProxyRegistry = utility_header::Registry<ServiceProxy>;

// The service proxy manager class that manages service proxies
class MONO_SERVICE_DLL ServiceProxyManager
{
public:
    ServiceProxyManager(ServiceProxyRegistry& service_proxy_registry) : 
        service_proxy_registry_(service_proxy_registry) 
    {
    }

    ~ServiceProxyManager() = default;

    // Execute a function with a unique lock on the service proxy map
    template <typename Func>
    void WithLock(Func&& func) 
    {
        service_proxy_registry_.WithUniqueLock([&](ServiceProxyRegistry& registry)
        {
            func(*this);
        });
    }

    // Get the service proxy
    ServiceProxy& GetServiceProxy(const ServiceHandleID& service_id);

    // Get the const service proxy
    const ServiceProxy& GetServiceProxy(const ServiceHandleID& service_id) const;

private:
    // The reference to the service proxy registry
    ServiceProxyRegistry& service_proxy_registry_;
};

} // namespace mono_service