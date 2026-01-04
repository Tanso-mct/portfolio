#pragma once

#include <unordered_set>
#include <memory>

#include "class_template/singleton.h"
#include "utility_header/container.h"
#include "directx12_util/include/wrapper.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/resource_handle.h"

namespace render_graph
{

// The container type used to contain resources
using ResourceContainer = utility_header::Container<dx12_util::Resource>;

class RENDER_GRAPH_DLL ResourceAccessToken
{
public:
    ResourceAccessToken() = default;
    ~ResourceAccessToken() = default;

    // Add a ResourceHandle to the set of accessible resources
    void PermitAccess(const ResourceHandle* handle);

    // Check if a ResourceHandle is in the set of accessible resources
    bool HasAccess(const ResourceHandle* handle) const;

    // Get the set of accessible ResourceHandles
    const std::unordered_set<const ResourceHandle*>& GetAccessibleResourceHandles() const;

private:
    // Set of ResourceHandles that are accessible
    std::unordered_set<const ResourceHandle*> accessible_resource_handles_;
};

// The resource manager class that manages resources and their handles
class RENDER_GRAPH_DLL ResourceManager :
    public class_template::Singleton<ResourceManager>
{
public:
    ResourceManager(ResourceContainer& container) : container_(container) {}
    ~ResourceManager() override = default;

    // Execute a function with a unique lock on the resource container
    template <typename Func>
    void WithLock(Func&& func) 
    {
        container_.WithUniqueLock([&](ResourceContainer& container) 
        {
            func(*this);
        });
    }

    // Get a resource for writing using its handle and a write access token
    // It must be called in WithLock lambda function
    dx12_util::Resource& GetWriteResource(const ResourceHandle* handle, const ResourceAccessToken& write_token);

    // Get a resource for reading using its handle and a read access token
    // It must be called in WithLock lambda function
    const dx12_util::Resource& GetReadResource(const ResourceHandle* handle, const ResourceAccessToken& read_token);

    // Wrap the Contains method of the container
    // It must be called in WithLock lambda function
    bool Contains(const ResourceHandle* handle) const;

private:
    // The resource container reference
    ResourceContainer& container_;
};

class RENDER_GRAPH_DLL ResourceAdder
{
public:
    ResourceAdder(ResourceContainer& container) : container_(container) {}
    ~ResourceAdder() = default;

    // Adds a new resource and returns its associated ResourceHandle
    ResourceHandle AddResource(std::unique_ptr<dx12_util::Resource> resource);

private:
    // The resource container reference
    ResourceContainer& container_;
};

class RENDER_GRAPH_DLL ResourceEraser
{
public:
    ResourceEraser(ResourceContainer& container) : container_(container) {}
    ~ResourceEraser() = default;

    // Erase a resource associated with the given ResourceHandle
    void EraseResource(const ResourceHandle* handle);

private:
    // The resource container reference
    ResourceContainer& container_;
};

} // namespace render_graph