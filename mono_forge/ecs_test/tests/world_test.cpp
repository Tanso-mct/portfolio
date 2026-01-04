#include "ecs_test/pch.h"

#include "ecs_test/tests/test_component.h"
#include "ecs/include/world.h"

TEST(World, CreateDestroyEntity)
{
    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create component descriptor registry
    std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry 
        = std::make_unique<ecs::ComponentDescriptorRegistry>();

    // Create the world
    ecs::World world(std::move(component_descriptor_registry));

    // Create an entity
    ecs::Entity entity = world.CreateEntity();
    ASSERT_TRUE(entity.IsValid());
    ASSERT_TRUE(world.CheckEntityExist(entity));

    // Destroy the entity
    bool destroyResult = world.DestroyEntity(entity);
    ASSERT_TRUE(destroyResult);
    ASSERT_FALSE(world.CheckEntityExist(entity));
}

TEST(World, AddRemoveComponent)
{
    bool result = false;

    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

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

    // Create an entity
    ecs::Entity entity = world.CreateEntity();
    ASSERT_TRUE(entity.IsValid());
    ASSERT_TRUE(world.CheckEntityExist(entity));

    // Create setup parameters for TestComponent
    std::unique_ptr<ecs_test::TestComponent::SetupParam> test_param 
        = std::make_unique<ecs_test::TestComponent::SetupParam>();

    // Set some value in the setup param
    test_param->value = 123;

    // Add TestComponent to the entity
    result = world.AddComponent<ecs_test::TestComponent>(
        entity, ecs_test::TestComponentHandle::ID(), std::move(test_param));
    ASSERT_TRUE(result);
    ASSERT_TRUE(world.HasComponent(entity, ecs_test::TestComponentHandle::ID()));

    // Get the component and check its data
    ecs_test::TestComponent* test_component 
        = world.GetComponent<ecs_test::TestComponent>(entity, ecs_test::TestComponentHandle::ID());
    ASSERT_NE(test_component, nullptr);
    ASSERT_EQ(test_component->GetData(), 123);

    // Remove the component from the entity
    result = world.RemoveComponent(entity, ecs_test::TestComponentHandle::ID());
    ASSERT_TRUE(result);
    ASSERT_FALSE(world.HasComponent(entity, ecs_test::TestComponentHandle::ID()));

    // Destroy the entity
    result = world.DestroyEntity(entity);
    ASSERT_TRUE(result);
    ASSERT_FALSE(world.CheckEntityExist(entity));
}

TEST(World, View)
{
    bool result = false;

    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

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

    // Create entity 1
    const int entity_1_data = 123;
    ecs::Entity entity1 = world.CreateEntity();
    {
        ASSERT_TRUE(entity1.IsValid());
        ASSERT_TRUE(world.CheckEntityExist(entity1));

        // Create setup parameters for TestComponent
        std::unique_ptr<ecs_test::TestComponent::SetupParam> test_param 
        = std::make_unique<ecs_test::TestComponent::SetupParam>();

        // Set some value in the setup param
        test_param->value = entity_1_data;

        // Add TestComponent to the entity1
        result = world.AddComponent<ecs_test::TestComponent>(
            entity1, ecs_test::TestComponentHandle::ID(), std::move(test_param));
        ASSERT_TRUE(result);
        ASSERT_TRUE(world.HasComponent(entity1, ecs_test::TestComponentHandle::ID()));

        // Get the component and check its data
        ecs_test::TestComponent* test_component 
            = world.GetComponent<ecs_test::TestComponent>(entity1, ecs_test::TestComponentHandle::ID());
        ASSERT_NE(test_component, nullptr);
        ASSERT_EQ(test_component->GetData(), 123);
    }

    // Create entity 2
    const int entity_2_data = 456;
    ecs::Entity entity2 = world.CreateEntity();
    {
        ASSERT_TRUE(entity2.IsValid());
        ASSERT_TRUE(world.CheckEntityExist(entity2));

        // Create setup parameters for TestComponent
        std::unique_ptr<ecs_test::TestComponent::SetupParam> test_param 
        = std::make_unique<ecs_test::TestComponent::SetupParam>();

        // Set some value in the setup param
        test_param->value = entity_2_data;

        // Add TestComponent to the entity2
        result = world.AddComponent<ecs_test::TestComponent>(
            entity2, ecs_test::TestComponentHandle::ID(), std::move(test_param));
        ASSERT_TRUE(result);
        ASSERT_TRUE(world.HasComponent(entity2, ecs_test::TestComponentHandle::ID()));

        // Get the component and check its data
        ecs_test::TestComponent* test_component 
            = world.GetComponent<ecs_test::TestComponent>(entity2, ecs_test::TestComponentHandle::ID());
        ASSERT_NE(test_component, nullptr);
        ASSERT_EQ(test_component->GetData(), 456);
    }

    for (const ecs::Entity& entity : world.View(ecs_test::TestComponentHandle::ID())())
    {
        ecs_test::TestComponent* test_component 
            = world.GetComponent<ecs_test::TestComponent>(entity, ecs_test::TestComponentHandle::ID());
        ASSERT_NE(test_component, nullptr);

        if (entity == entity1)
            ASSERT_EQ(test_component->GetData(), entity_1_data);
        else if (entity == entity2)
            ASSERT_EQ(test_component->GetData(), entity_2_data);
        else
            FAIL() << "Unexpected entity in view";
    }
}