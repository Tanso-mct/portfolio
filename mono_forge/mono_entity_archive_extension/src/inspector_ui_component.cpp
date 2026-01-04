#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/inspector_ui_component.h"

namespace mono_entity_archive_extension
{

InspectorUIComponent::InspectorUIComponent()
{
}

InspectorUIComponent::~InspectorUIComponent()
{
}

bool InspectorUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    InspectorUIComponent::SetupParam* inspector_ui_component_param
        = dynamic_cast<InspectorUIComponent::SetupParam*>(&param);
    assert(inspector_ui_component_param != nullptr && "Invalid setup param type for InspectorUIComponent");

    // Set parameters


    return true; // Success
}

ecs::ComponentID InspectorUIComponent::GetID() const
{
    return InspectorUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension