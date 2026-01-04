#include "component_editor/src/pch.h"
#include "component_editor/include/component_name_registrar.h"

namespace component_editor
{

ComponentNameRegistrar::ComponentNameRegistrar(
    ComponentNameMap& name_map,
    ecs::ComponentID component_id, std::string component_name)
{
    assert(
        name_map.find(component_id) == name_map.end() && 
        "Component ID already registered in ComponentNameRegistrar");

    // Register the component name in the map
    name_map[component_id] = component_name;
}

} // namespace component_editor