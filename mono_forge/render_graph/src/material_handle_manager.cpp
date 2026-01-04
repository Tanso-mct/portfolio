#include "render_graph/src/pch.h"
#include "render_graph/include/material_handle_manager.h"

namespace render_graph
{

void MaterialHandleManager::RegisterMaterialHandle(
    MaterialHandleKey material_handle_id, render_graph::MaterialHandle material_handle, std::string_view material_name)
{
    assert(
        material_handles_.find(material_handle_id) == material_handles_.end()
        && "Material handle ID already registered.");

    // Register the material handle in the map
    material_handles_[material_handle_id] = material_handle;

    // Register the material name in the map
    material_names_[material_handle_id] = std::string(material_name);
}

void MaterialHandleManager::UnregisterMaterialHandle(MaterialHandleKey material_handle_id)
{
    auto it = material_handles_.find(material_handle_id);
    assert(it != material_handles_.end() && "Material handle ID not found.");

    // Erase the material handle from the map
    material_handles_.erase(it);

    // Erase the material name from the map
    material_names_.erase(material_handle_id);
}

render_graph::MaterialHandle* MaterialHandleManager::GetMaterialHandle(MaterialHandleKey material_handle_id)
{
    auto it = material_handles_.find(material_handle_id);
    assert(it != material_handles_.end() && "Material handle ID not found.");

    return &it->second;
}

std::string_view MaterialHandleManager::GetMaterialHandleName(MaterialHandleKey material_handle_id) const
{
    auto it = material_names_.find(material_handle_id);
    assert(it != material_names_.end() && "Material handle ID not found.");

    return it->second;
}

std::vector<MaterialHandleKey> MaterialHandleManager::GetRegisteredMaterialHandleKeys() const
{
    std::vector<MaterialHandleKey> ids;
    ids.reserve(material_handles_.size());

    for (const auto& pair : material_handles_)
        ids.push_back(pair.first);

    return ids;
}

void MaterialHandleManager::SetMaterialHandleName(MaterialHandleKey material_handle_id, std::string_view material_name)
{
    auto it = material_names_.find(material_handle_id);
    assert(it != material_names_.end() && "Material handle ID not found.");

    it->second = std::string(material_name);
}

} // namespace render_graph