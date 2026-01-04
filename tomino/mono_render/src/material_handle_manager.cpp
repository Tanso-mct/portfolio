#include "mono_render/src/pch.h"
#include "mono_render/include/material_handle_manager.h"

namespace mono_render
{

void MaterialHandleManager::RegisterMaterialHandle(
    uint32_t material_handle_id, render_graph::MaterialHandle material_handle)
{
    if (material_handles_.find(material_handle_id) != material_handles_.end())
        return; // Already registered

    // Register the material handle in the map
    material_handles_[material_handle_id] = material_handle;
}

render_graph::MaterialHandle* MaterialHandleManager::GetMaterialHandle(uint32_t material_handle_id)
{
    auto it = material_handles_.find(material_handle_id);
    assert(it != material_handles_.end() && "Material handle ID not found.");

    return &it->second;
}

} // namespace mono_render