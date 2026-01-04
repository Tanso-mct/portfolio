#include "render_graph/src/pch.h"
#include "render_graph/include/material_manager.h"

namespace render_graph
{

Material& MaterialManager::GetMaterial(const MaterialHandle* handle)
{
    assert(handle != nullptr && "MaterialHandle pointer is null");
    return container_.Get(*handle);
}

const Material& MaterialManager::GetMaterial(const MaterialHandle* handle) const
{
    assert(handle != nullptr && "MaterialHandle pointer is null");
    return container_.Get(*handle);
}

bool MaterialManager::Contains(const MaterialHandle* handle) const
{
    assert(handle != nullptr && "MaterialHandle pointer is null");
    return container_.Contains(*handle);
}

MaterialHandle MaterialAdder::AddMaterial(std::unique_ptr<Material> material)
{
    MaterialHandle handle;
    container_.WithUniqueLock([&](MaterialContainer& container) 
    {
        // Add the material and get its handle
        handle = container.Add(std::move(material));
    });

    return handle;
}

void MaterialEraser::EraseMaterial(const MaterialHandle* handle)
{
    assert(handle != nullptr && "MaterialHandle pointer is null");

    container_.WithUniqueLock([&](MaterialContainer& container) 
    {
        container.Erase(*handle);
    });
}

} // namespace render_graph