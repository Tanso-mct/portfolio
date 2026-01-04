#include "windows_base/src/pch.h"
#include "windows_base/include/monitor_registry.h"

void wb::MonitorRegistry::Add(size_t id, size_t factoryID)
{
    if (factoryIDs_.find(id) != factoryIDs_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Monitor with ID ", std::to_string(id), " already exists."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Add the factory ID to the registry
    factoryIDs_[id] = factoryID;

    // Save the key
    keys_.push_back(id);

    // Update the maximum ID if necessary
    if (id > maxId)
    {
        maxId = id;
    }
}

const size_t &wb::MonitorRegistry::GetFactoryID(size_t id) const
{
    if (factoryIDs_.find(id) == factoryIDs_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Monitor Factory for ID ", std::to_string(id), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return factoryIDs_.at(id);
}

size_t wb::MonitorRegistry::GetMaxID() const
{
    return maxId;
}

const std::vector<size_t> &wb::MonitorRegistry::GetKeys() const
{
    return keys_;
}

WINDOWS_BASE_API wb::MonitorRegistry wb::gMonitorRegistry;

wb::MonitorRegistrar::MonitorRegistrar(size_t monitorID, size_t factoryID)
{
    wb::gMonitorRegistry.Add(monitorID, factoryID);
}