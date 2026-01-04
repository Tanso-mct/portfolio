#pragma once

#include "component_editor/include/dll_config.h"
#include "component_editor/include/component_adder.h"

namespace component_editor
{

// Registrar class to register component adders in constructor
class COMPONENT_EDITOR_DLL ComponentAdderRegistrar
{
public:
    ComponentAdderRegistrar(
        ComponentAdderMap& adder_map,
        ecs::ComponentID component_id, std::unique_ptr<ComponentAdder> adder);
    ~ComponentAdderRegistrar() = default;
};

} // namespace component_editor