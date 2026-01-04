#pragma once

#include "component_editor/include/dll_config.h"
#include "component_editor/include/setup_param_registry.h"

namespace component_editor
{

// The manager for get setup parameters of components for entities
class COMPONENT_EDITOR_DLL SetupParamManager
{
public:
    SetupParamManager(EntitySetupParamMap& entity_setup_param_map) : 
        entity_setup_param_map_(entity_setup_param_map)
    {
    }

    virtual ~SetupParamManager() = default;

    // Get the entity's component's setup parameters
    ecs::Component::SetupParam* GetSetupParam(ecs::Entity entity, ecs::ComponentID component_id);

    // Get the entity's component's setup parameters (const version)
    const ecs::Component::SetupParam* GetSetupParam(ecs::Entity entity, ecs::ComponentID component_id) const;

    // Check if the entity's component has setup parameters
    bool ContainSetupParam(ecs::Entity entity, ecs::ComponentID component_id);

private:
    // The reference to the entity setup parameter map
    EntitySetupParamMap& entity_setup_param_map_;
};

// Add setup parameters for components of entities
class COMPONENT_EDITOR_DLL SetupParamAdder
{
public:
    SetupParamAdder(EntitySetupParamMap& entity_setup_param_map) : 
        entity_setup_param_map_(entity_setup_param_map)
    {
    }

    virtual ~SetupParamAdder() = default;

    // Add a setup parameter for the entity's component
    void AddSetupParam(
        ecs::Entity entity, ecs::ComponentID component_id, 
        std::unique_ptr<ecs::Component::SetupParam> setup_param);

private:
    // The reference to the entity setup parameter map
    EntitySetupParamMap& entity_setup_param_map_;
};

// Erase setup parameters for components of entities
class COMPONENT_EDITOR_DLL SetupParamEraser
{
public:
    SetupParamEraser(EntitySetupParamMap& entity_setup_param_map) : 
        entity_setup_param_map_(entity_setup_param_map)
    {
    }

    virtual ~SetupParamEraser() = default;

    // Erase the setup parameter for the entity's component
    void EraseSetupParam(ecs::Entity entity, ecs::ComponentID component_id);

    // Erase all setup parameters for the entity
    void EraseSetupParam(ecs::Entity entity);

private:
    // The reference to the entity setup parameter map
    EntitySetupParamMap& entity_setup_param_map_;
};

} // namespace component_editor