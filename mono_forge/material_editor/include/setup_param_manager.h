#pragma once

#include "material_editor/include/dll_config.h"
#include "material_editor/include/setup_param_map.h"

namespace material_editor
{

// The manager for get setup parameters of components for entities
class MATERIAL_EDITOR_DLL SetupParamManager
{
public:
    SetupParamManager(SetupParamMap& setup_param_map) : 
        setup_param_map_(setup_param_map)
    {
    }

    virtual ~SetupParamManager() = default;

    // Get the material's setup parameters
    SetupParamWrapper* GetSetupParam(render_graph::MaterialHandle* material_handle);

    // Get the material's setup parameters (const version)
    const SetupParamWrapper* GetSetupParam(render_graph::MaterialHandle* material_handle) const;

    // Check if the material has setup parameters
    bool ContainSetupParam(render_graph::MaterialHandle* material_handle);

private:
    // The reference to the entity setup parameter map
    SetupParamMap& setup_param_map_;
};

// Add setup parameters for components of entities
class MATERIAL_EDITOR_DLL SetupParamAdder
{
public:
    SetupParamAdder(SetupParamMap& setup_param_map) : 
        setup_param_map_(setup_param_map)
    {
    }

    virtual ~SetupParamAdder() = default;

    // Add a setup parameter for the material
    void AddSetupParam(
        render_graph::MaterialHandle* material_handle, 
        std::unique_ptr<SetupParamWrapper> setup_param);

private:
    // The reference to the entity setup parameter map
    SetupParamMap& setup_param_map_;
};

// Erase setup parameters for components of entities
class MATERIAL_EDITOR_DLL SetupParamEraser
{
public:
    SetupParamEraser(SetupParamMap& setup_param_map) : 
        setup_param_map_(setup_param_map)
    {
    }

    virtual ~SetupParamEraser() = default;

    // Erase the setup parameter for the material
    void EraseSetupParam(render_graph::MaterialHandle material_handle);

private:
    // The reference to the entity setup parameter map
    SetupParamMap& setup_param_map_;
};

} // namespace material_editor