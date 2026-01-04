#include "component_editor/src/pch.h"
#include "component_editor/include/setup_param_manager.h"

namespace component_editor
{

ecs::Component::SetupParam* SetupParamManager::GetSetupParam(ecs::Entity entity, ecs::ComponentID component_id)
{
    assert(entity.IsValid() && "Entity is not valid");

    // Check if the entity exists in the map
    auto entity_it = entity_setup_param_map_.find(entity);
    assert(entity_it != entity_setup_param_map_.end() && "Entity not found in setup param map");

    // Get the component setup param map for the entity
    ComponentSetupParamMap& component_map = entity_setup_param_map_[entity];

    // Check if the component ID exists in the component map
    auto component_it = component_map.find(component_id);
    assert(component_it != component_map.end() && "Component ID not found in setup param map for entity");

    // Return the setup parameter pointer
    return component_map[component_id].get();
}

const ecs::Component::SetupParam* SetupParamManager::GetSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id) const
{
    assert(entity.IsValid() && "Entity is not valid");

    // Check if the entity exists in the map
    auto entity_it = entity_setup_param_map_.find(entity);
    assert(entity_it != entity_setup_param_map_.end() && "Entity not found in setup param map");

    // Get the component setup param map for the entity
    const ComponentSetupParamMap& component_map = entity_it->second;

    // Check if the component ID exists in the component map
    auto component_it = component_map.find(component_id);
    assert(component_it != component_map.end() && "Component ID not found in setup param map for entity");

    // Return the setup parameter pointer
    return component_map.at(component_id).get();
}

bool SetupParamManager::ContainSetupParam(ecs::Entity entity, ecs::ComponentID component_id)
{
    assert(entity.IsValid() && "Entity is not valid");

    // Check if the entity exists in the map
    auto entity_it = entity_setup_param_map_.find(entity);
    if (entity_it == entity_setup_param_map_.end())
        return false;

    // Get the component setup param map for the entity
    ComponentSetupParamMap& component_map = entity_it->second;

    // Check if the component ID exists in the component map
    auto component_it = component_map.find(component_id);
    return component_it != component_map.end();
}

void SetupParamAdder::AddSetupParam(
    ecs::Entity entity, ecs::ComponentID component_id, 
    std::unique_ptr<ecs::Component::SetupParam> setup_param)
{
    assert(entity.IsValid() && "Entity is not valid");
    assert(setup_param != nullptr && "Setup param is null");

    // Add or update the setup parameter for the component ID
    entity_setup_param_map_[entity][component_id] = std::move(setup_param);
}

void SetupParamEraser::EraseSetupParam(ecs::Entity entity, ecs::ComponentID component_id)
{
    assert(entity.IsValid() && "Entity is not valid");

    // Find the entity in the map
    auto entity_it = entity_setup_param_map_.find(entity);
    if (entity_it != entity_setup_param_map_.end())
    {
        // Find the component ID in the component map
        ComponentSetupParamMap& component_map = entity_it->second;
        auto component_it = component_map.find(component_id);

        // If the component ID exists, erase it
        if (component_it != component_map.end())
        {
            // Erase the setup parameter for the component ID
            component_map.erase(component_it);

            // If the component map is empty after erasure, remove the entity entry
            if (component_map.empty())
                entity_setup_param_map_.erase(entity_it);
        }
    }
}

void SetupParamEraser::EraseSetupParam(ecs::Entity entity)
{
    assert(entity.IsValid() && "Entity is not valid");

    // Erase all setup parameters for the entity
    entity_setup_param_map_.erase(entity);
}

} // namespace component_editor