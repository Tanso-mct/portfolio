#pragma once

#include <memory>

#include "mono_service/include/dll_config.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_registry.h"


namespace mono_service
{

// The service importer class
class MONO_SERVICE_DLL ServiceImporter
{
public:
    ServiceImporter(ServiceRegistry& registry);
    virtual ~ServiceImporter() = default;

    // Import a service into the registry
    // The service will be set up with the provided setup parameters
    // service: The service instance to import
    // handle_id: The handle ID to register the service with
    // setup_param: The setup parameters to use for setting up the service
    bool Import(
        std::unique_ptr<Service> service, ServiceHandleID handle_id, Service::SetupParam& setup_param);

private:
    // The service registry to import services into
    ServiceRegistry& registry_;
};

template <typename ServiceType, typename HandleType>
bool ImportService(
    mono_service::ServiceRegistry& service_registry, 
    ServiceThreadAffinityID thread_affinity_id, Service::SetupParam& setup_param)
{
    // Create service importer
    std::unique_ptr<mono_service::ServiceImporter> service_importer 
        = std::make_unique<mono_service::ServiceImporter>(service_registry);

    // Create service instance
    std::unique_ptr<ServiceType> service 
        = std::make_unique<ServiceType>(thread_affinity_id);

    // Import service into registry
    return service_importer->Import(
        std::move(service), HandleType::ID(), setup_param);
}

} // namespace mono_service