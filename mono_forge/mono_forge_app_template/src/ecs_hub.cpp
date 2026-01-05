#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/ecs_hub.h"

namespace mono_forge_app_template
{

ECSHub::ECSHub()
{
}

ECSHub::~ECSHub()
{
    // Cleanup
    ecs_world.reset();
    systems.clear();
}

void ECSHub::CreateWorld(std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry)
{
    assert(ecs_world == nullptr && "ECS world already created!");
    assert(component_descriptor_registry != nullptr && "Component descriptor registry is null!");

    // Create the ecs world
    ecs_world = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
}

bool ECSHub::Update(const std::vector<ecs::SystemID>& update_order)
{
    assert(ecs_world != nullptr && "ECS world is not created!");

    for (const auto& system_id : update_order)
    {
        // Get system
        ecs::System& system = *systems.at(system_id);

        // Pre-update
        if (!system.PreUpdate(*ecs_world))
            return false;
    }

    for (const auto& system_id : update_order)
    {
        // Get system
        ecs::System& system = *systems.at(system_id);

        // Update
        if (!system.Update(*ecs_world))
            return false;
    }

    for (const auto& system_id : update_order)
    {
        // Get system
        ecs::System& system = *systems.at(system_id);

        // Post-update
        if (!system.PostUpdate(*ecs_world))
            return false;
    }

    return true; // Success
}

ecs::World& ECSHub::GetWorld()
{
    assert(ecs_world != nullptr && "ECS world is not created!");
    return *ecs_world;
}

} // namespace mono_forge_app_template