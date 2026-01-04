#include "material_editor/src/pch.h"
#include "material_editor/include/setup_param_manager.h"

namespace material_editor
{

SetupParamWrapper* SetupParamManager::GetSetupParam(render_graph::MaterialHandle* material_handle)
{
    auto it = setup_param_map_.find(*material_handle);
    if (it != setup_param_map_.end())
        return it->second.get();

    return nullptr;
}

const SetupParamWrapper* SetupParamManager::GetSetupParam(
    render_graph::MaterialHandle* material_handle) const
{
    auto it = setup_param_map_.find(*material_handle);
    if (it != setup_param_map_.end())
        return it->second.get();

    return nullptr;
}

bool SetupParamManager::ContainSetupParam(render_graph::MaterialHandle* material_handle)
{
    auto it = setup_param_map_.find(*material_handle);
    return it != setup_param_map_.end();
}

void SetupParamAdder::AddSetupParam(
    render_graph::MaterialHandle* material_handle, 
    std::unique_ptr<SetupParamWrapper> setup_param)
{
    assert(material_handle->IsValid() && "Material handle is not valid");
    assert(setup_param != nullptr && "Setup param is null");

    setup_param_map_[*material_handle] = std::move(setup_param);
}

void SetupParamEraser::EraseSetupParam(render_graph::MaterialHandle material_handle)
{
    assert(material_handle.IsValid() && "Material handle is not valid");

    auto it = setup_param_map_.find(material_handle);
    if (it != setup_param_map_.end())

    setup_param_map_.erase(material_handle);
}

} // namespace material_editor