#include "component_editor/src/pch.h"
#include "component_editor/include/component_adder_registrar.h"

namespace component_editor
{

ComponentAdderRegistrar::ComponentAdderRegistrar(
    ComponentAdderMap& adder_map,
    ecs::ComponentID component_id, std::unique_ptr<ComponentAdder> adder)
{
    assert(
        adder_map.find(component_id) == adder_map.end() &&
        "Component adder for this component ID is already registered.");

    // Register the component adder in the map
    adder_map.emplace(component_id, std::move(adder));
}

} // namespace component_editor