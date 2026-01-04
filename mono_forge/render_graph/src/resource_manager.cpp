#include "render_graph/src/pch.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

void ResourceAccessToken::PermitAccess(const ResourceHandle* handle)
{
    assert(handle != nullptr && "ResourceHandle pointer is null.");
    accessible_resource_handles_.insert(handle);
}

bool ResourceAccessToken::HasAccess(const ResourceHandle* handle) const
{
    assert(handle != nullptr && "ResourceHandle pointer is null.");
    return accessible_resource_handles_.find(handle) != accessible_resource_handles_.end();
}

const std::unordered_set<const ResourceHandle*>& ResourceAccessToken::GetAccessibleResourceHandles() const
{
    return accessible_resource_handles_;
}

dx12_util::Resource& ResourceManager::GetWriteResource(
    const ResourceHandle* handle, const ResourceAccessToken& write_token)
{
    assert(write_token.HasAccess(handle)); // Ensure the token has access to the resource
    return container_.Get(*handle);
}

const dx12_util::Resource& ResourceManager::GetReadResource(
    const ResourceHandle* handle, const ResourceAccessToken& read_token)
{
    assert(read_token.HasAccess(handle)); // Ensure the token has access to the resource
    return container_.Get(*handle);
}

bool ResourceManager::Contains(const ResourceHandle* handle) const
{
    assert(handle != nullptr && "ResourceHandle pointer is null.");
    return container_.Contains(*handle);
}

ResourceHandle ResourceAdder::AddResource(std::unique_ptr<dx12_util::Resource> resource)
{
    ResourceHandle handle;
    container_.WithUniqueLock([&](ResourceContainer& container) 
    {
        // Add the resource and get its handle
        handle = container.Add(std::move(resource));
    });

    return handle;
}

void ResourceEraser::EraseResource(const ResourceHandle* handle)
{
    assert(handle != nullptr && "ResourceHandle pointer is null.");

    container_.WithUniqueLock([&](ResourceContainer& container) 
    {
        container.Erase(*handle);
    });
}

} // namespace render_graph