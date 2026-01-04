#pragma once

#include "component_editor/include/dll_config.h"
#include "component_editor/include/component_reflection_registry.h"

namespace component_editor
{

// Registrar class to register component names in constructor
class COMPONENT_EDITOR_DLL ComponentNameRegistrar
{
public:
    ComponentNameRegistrar(
        ComponentNameMap& name_map,
        ecs::ComponentID component_id, std::string component_name);
    ~ComponentNameRegistrar() = default;
};

} // namespace component_editor