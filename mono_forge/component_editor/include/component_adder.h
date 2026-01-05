#pragma once

#include <cassert>

#include "ecs/include/world.h"
#include "mono_service/include/service_proxy_manager.h"

namespace component_editor
{

// Interface for adding components to the ECS world
// This class provides a method to add a component which is default-constructed and uses default setup parameters
class ComponentAdder
{
public:
    ComponentAdder() = default;
    virtual ~ComponentAdder() = default;

    // Add a component to the world for the specified entity with default setup parameters and constructor arguments
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity, 
        mono_service::ServiceProxyManager& service_proxy_manager) const = 0;

    // Add a component to the world for the specified entity with given setup parameters and default constructor arguments
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity, 
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const
    {
        assert(false && "Add with setup_param not implemented for this component adder");
        return false;
    }

    // Get default setup param for the component
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const
    {
        assert(false && "GetSetupParam not implemented for this component adder");
        return nullptr;
    }
};

// Map from component ID to component adder
using ComponentAdderMap = std::unordered_map<ecs::ComponentID, std::unique_ptr<ComponentAdder>>;

} // namespace component_editor