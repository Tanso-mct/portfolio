#include "mono_entity_archive_service_test/pch.h"
#include "mono_entity_archive_service_test/tests/test_meta_component.h"

namespace component_editor_test
{

TestMetaComponent::TestMetaComponent()
{
}

TestMetaComponent::~TestMetaComponent()
{
}

bool TestMetaComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    TestMetaComponent::SetupParam* test_meta_component_param
        = dynamic_cast<TestMetaComponent::SetupParam*>(&param);
    assert(test_meta_component_param != nullptr && "Invalid setup param type for TestMetaComponent");

    // Set parameters
    name_ = test_meta_component_param->name;
    active_self_ = test_meta_component_param->active_self;
    tag_ = test_meta_component_param->tag;
    layer_ = test_meta_component_param->layer;

    return true; // Success
}

bool TestMetaComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const TestMetaComponent::SetupParam* test_meta_component_param
        = dynamic_cast<const TestMetaComponent::SetupParam*>(&param);
    assert(test_meta_component_param != nullptr && "Invalid setup param type for TestMetaComponent");

    // Apply changes
    name_ = test_meta_component_param->name;
    active_self_ = test_meta_component_param->active_self;
    tag_ = test_meta_component_param->tag;
    layer_ = test_meta_component_param->layer;

    return true; // Success
}

ecs::ComponentID TestMetaComponent::GetID() const
{
    return TestMetaComponentHandle::ID();
}

bool TestMetaComponentAdder::Add(
    ecs::World& world, const ecs::Entity& entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param
    std::unique_ptr<TestMetaComponent::SetupParam> setup_param 
        = std::make_unique<TestMetaComponent::SetupParam>();

    // Add component to the world
    return world.AddComponent<TestMetaComponent>(
        entity, TestMetaComponentHandle::ID(), std::move(setup_param));
}

std::unique_ptr<ecs::Component::SetupParam> TestMetaComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    return std::make_unique<TestMetaComponent::SetupParam>();
}

} // namespace component_editor_test