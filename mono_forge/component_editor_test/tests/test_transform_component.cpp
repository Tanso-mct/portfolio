#include "component_editor_test/pch.h"
#include "component_editor_test/tests/test_transform_component.h"

namespace component_editor_test
{

TestTransformComponent::TestTransformComponent()
{
}

TestTransformComponent::~TestTransformComponent()
{
}

bool TestTransformComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    TestTransformComponent::SetupParam* test_transform_component_param
        = dynamic_cast<TestTransformComponent::SetupParam*>(&param);
    assert(test_transform_component_param != nullptr && "Invalid setup param type for TestTransformComponent");

    // Set parameters
    position_ = test_transform_component_param->position;
    rotation_ = test_transform_component_param->rotation;
    scale_ = test_transform_component_param->scale;

    return true; // Success
}

bool TestTransformComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const TestTransformComponent::SetupParam* test_transform_component_param
        = dynamic_cast<const TestTransformComponent::SetupParam*>(&param);
    assert(test_transform_component_param != nullptr && "Invalid setup param type for TestTransformComponent");

    // Apply changes
    position_ = test_transform_component_param->position;
    rotation_ = test_transform_component_param->rotation;
    scale_ = test_transform_component_param->scale;

    return true; // Success
}

ecs::ComponentID TestTransformComponent::GetID() const
{
    return TestTransformComponentHandle::ID();
}

bool TestTransformComponentAdder::Add(
    ecs::World& world, const ecs::Entity& entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param
    std::unique_ptr<TestTransformComponent::SetupParam> setup_param 
        = std::make_unique<TestTransformComponent::SetupParam>();

    // Add component to the world
    return world.AddComponent<TestTransformComponent>(
        entity, TestTransformComponentHandle::ID(), std::move(setup_param));
}

} // namespace component_editor_test