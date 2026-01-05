#pragma once

#include <memory>
#include <unordered_map>

#include "ecs/include/system.h"
#include "ecs/include/world.h"

namespace mono_forge_app_template
{

// The ECS hub class that manages systems and world
class ECSHub
{
public:
    ECSHub();
    ~ECSHub();

    // Register a system
    template <typename SystemType, typename... Args>
    void RegisterSystem(Args&&... args)
    {
        // Create system
        std::unique_ptr<ecs::System> system
            = std::make_unique<SystemType>(std::forward<Args>(args)...);

        // Get system ID
        ecs::SystemID system_id = system->GetID();

        // Check if system ID already exists
        assert(systems.find(system_id) == systems.end() && "System ID already registered!");

        // Register system
        systems.emplace(system_id, std::move(system));
    }

    // Create the ECS world
    void CreateWorld(std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry);

    // Update systems in the given order
    bool Update(const std::vector<ecs::SystemID>& update_order);

    // Get the ECS world
    ecs::World& GetWorld();

private:
    // The system map
    std::unordered_map<ecs::SystemID, std::unique_ptr<ecs::System>> systems;

    // The ECS world
    std::unique_ptr<ecs::World> ecs_world = nullptr;
};

} // namespace mono_forge_app_template