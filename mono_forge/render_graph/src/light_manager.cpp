#include "render_graph/src/pch.h"
#include "render_graph/include/light_manager.h"

namespace render_graph
{

Light& LightManager::GetLight(const LightHandle* handle)
{
    assert(handle != nullptr && "LightHandle pointer is null");
    return container_.Get(*handle);
}

const Light& LightManager::GetLight(const LightHandle* handle) const
{
    assert(handle != nullptr && "LightHandle pointer is null");
    return container_.Get(*handle);
}

bool LightManager::Contains(const LightHandle* handle) const
{
    assert(handle != nullptr && "LightHandle pointer is null");
    return container_.Contains(*handle);
}

LightHandle LightAdder::AddLight(std::unique_ptr<Light> resource)
{
    LightHandle handle;
    container_.WithUniqueLock([&](LightContainer& container) 
    {
        // Add the light and get its handle
        handle = container.Add(std::move(resource));
    });

    return handle;
}

void LightEraser::EraseLight(const LightHandle* handle)
{
    assert(handle != nullptr && "LightHandle pointer is null");

    container_.WithUniqueLock([&](LightContainer& container) 
    {
        container.Erase(*handle);
    });
}

} // namespace render_graph