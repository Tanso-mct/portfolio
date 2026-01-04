#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/transform_manipulator_ui_component.h"

namespace mono_entity_archive_extension
{

TransformManipulatorUIComponent::TransformManipulatorUIComponent()
{
}

TransformManipulatorUIComponent::~TransformManipulatorUIComponent()
{
}

bool TransformManipulatorUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    TransformManipulatorUIComponent::SetupParam* transform_manipulator_ui_component_param
        = dynamic_cast<TransformManipulatorUIComponent::SetupParam*>(&param);
    assert(transform_manipulator_ui_component_param != nullptr && "Invalid setup param type for TransformManipulatorUIComponent");

    // Set parameters


    return true; // Success
}

bool TransformManipulatorUIComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const TransformManipulatorUIComponent::SetupParam* transform_manipulator_ui_component_param
        = dynamic_cast<const TransformManipulatorUIComponent::SetupParam*>(&param);
    assert(transform_manipulator_ui_component_param != nullptr && "Invalid setup param type for TransformManipulatorUIComponent");

    // Set parameters

    return true; // Success
}

ecs::ComponentID TransformManipulatorUIComponent::GetID() const
{
    return TransformManipulatorUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension