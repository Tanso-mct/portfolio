#include "mono_service/src/pch.h"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_service
{

ServiceProxy& ServiceProxyManager::GetServiceProxy(const ServiceHandleID& service_id)
{
    return service_proxy_registry_.Get(service_id);
}

const ServiceProxy& ServiceProxyManager::GetServiceProxy(const ServiceHandleID& service_id) const
{
    return service_proxy_registry_.Get(service_id);
}

} // namespace mono_service