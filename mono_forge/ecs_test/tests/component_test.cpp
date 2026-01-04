#include "ecs_test/pch.h"

#include "ecs_test/tests/test_component.h"

TEST(Component, Setup)
{
    // Create singleton instance of ComponentIDGenerator
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator 
        = std::make_unique<ecs::ComponentIDGenerator>();

    // Create component memory pool
    memory_allocator::Pool test_component_pool(sizeof(ecs_test::TestComponent) * 10);

    // Create component allocator factory
    ecs_test::TestComponentAllocatorFactory test_component_allocator_factory;

    // Create component allocator
    std::unique_ptr<memory_allocator::Allocator> test_component_allocator
        = test_component_allocator_factory.Create(test_component_pool, sizeof(ecs_test::TestComponent));

    // Allocate memory for the component
    std::byte* component_memory = test_component_allocator->Allocate(sizeof(ecs_test::TestComponent));
    ASSERT_NE(component_memory, nullptr);

    // Create the component instance in the allocated memory
    ecs::Component* component = new (component_memory) ecs_test::TestComponent();

    // Setup the component with valid parameters
    ecs_test::TestComponent::SetupParam param;
    param.value = 42;
    bool setup_result = component->Setup(param);
    EXPECT_TRUE(setup_result);

    // Verify that the component data is set correctly
    ecs_test::TestComponent* test_component = dynamic_cast<ecs_test::TestComponent*>(component);
    ASSERT_NE(test_component, nullptr);
    EXPECT_EQ(test_component->GetData(), 42);

    // Call destructor explicitly
    component->~Component();

    // Free the memory allocated for the component using the allocator
    test_component_allocator->Deallocate(reinterpret_cast<std::byte**>(&component));
    EXPECT_EQ(component, nullptr);
}