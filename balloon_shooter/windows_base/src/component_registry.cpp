#include "windows_base/src/pch.h"
#include "windows_base/include/component_registry.h"

void wb::ComponentRegistry::AddFactory(size_t componentID, std::unique_ptr<IComponentFactory> componentFactory)
{
    if (componentFactories_.find(componentID) != componentFactories_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Component factory with ID ", std::to_string(componentID), " already exists."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Add the factory to the registry
    componentFactories_[componentID] = std::move(componentFactory);

    // Save the key
    keys_.push_back(componentID);

    // Update the maximum ID if necessary
    if (componentID > maxId)
    {
        maxId = componentID;
    }
}

wb::IComponentFactory &wb::ComponentRegistry::GetFactory(size_t componentID)
{
    if (componentFactories_.find(componentID) == componentFactories_.end())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Component factory with ID ", std::to_string(componentID), " does not exist."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return *componentFactories_[componentID];
}

size_t wb::ComponentRegistry::GetMaxID() const
{
    return maxId;
}

const std::vector<size_t> &wb::ComponentRegistry::GetKeys() const
{
    return keys_;
}

WINDOWS_BASE_API wb::ComponentRegistry wb::gComponentRegistry;

wb::ComponentRegistrar::ComponentRegistrar(size_t componentID, std::unique_ptr<IComponentFactory> componentFactory)
{
    wb::gComponentRegistry.AddFactory(componentID, std::move(componentFactory));
}