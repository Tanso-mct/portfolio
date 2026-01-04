#include "ecs_test/pch.h"

#include "ecs_test/tests/test_component.h"
#include "ecs_test/tests/test_system.h"
#include "ecs/include/world.h"

TEST(System, Update)
{
    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create singleton instance of SystemIDGenerator
    std::unique_ptr<ecs::SystemIDGenerator> system_id_generator 
        = std::make_unique<ecs::SystemIDGenerator>();

    // Create component descriptor registry
    std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry 
        = std::make_unique<ecs::ComponentDescriptorRegistry>();

    // Register TestComponent
    {
        // Create descriptor for TestComponent
        std::unique_ptr<ecs::ComponentDescriptor> test_desc = std::make_unique<ecs::ComponentDescriptor>(
            sizeof(ecs_test::TestComponent), // Component size
            10, // Max count
            std::make_unique<ecs_test::TestComponentAllocatorFactory>());

        component_descriptor_registry->WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
        {
            // Register the descriptor with component ID
            registry.Register(ecs_test::TestComponentHandle::ID(), std::move(test_desc));
        });
    }

    // Create the world
    ecs::World world(std::move(component_descriptor_registry));

    // Prepare systems
    std::vector<std::unique_ptr<ecs::System>> systems;
    systems.push_back(std::make_unique<ecs_test::ExampleASystem>()); // Example A
    systems.push_back(std::make_unique<ecs_test::ExampleBSystem>()); // Example B

    // Update the systems
    for (std::unique_ptr<ecs::System>& system : systems)
    {
        bool result = system->Update(world);
        ASSERT_TRUE(result); // Ensure the update was successful
    }
}