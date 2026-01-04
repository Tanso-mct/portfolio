#include "mono_service/src/pch.h"
#include "mono_service/include/service_importer.h"

#include "utility_header/logger.h"

namespace mono_service
{

mono_service::ServiceImporter::ServiceImporter(ServiceRegistry& registry) :
    registry_(registry)
{
}

bool mono_service::ServiceImporter::Import(
    std::unique_ptr<Service> service, ServiceHandleID handle_id, Service::SetupParam &setup_param)
{
    // Setup the service
    if (!service->Setup(setup_param))
    {
        utility_header::ConsoleLogErr(
            {"Failed to setup service during import."}, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Register the service in the registry
    bool result = true;
    registry_.WithUniqueLock([&](ServiceRegistry& registry)
    {
        if (registry.Contains(handle_id))
        {
            utility_header::ConsoleLogErr(
                {"Service with the given handle ID is already registered."}, __FILE__, __LINE__, __FUNCTION__);

            result = false;
            return;
        }

        // Register the service
        registry.Register(handle_id, std::move(service));
    });

    // Check if registration succeeded
    if (!result)
        return false;

    return true; // Successfully imported the service
}

} // namespace mono_service