#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/editor_enabled_component.h"

namespace mono_entity_archive_extension
{

EditorEnabledComponent::EditorEnabledComponent()
{
}

EditorEnabledComponent::~EditorEnabledComponent()
{
}

bool EditorEnabledComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    EditorEnabledComponent::SetupParam* editor_enabled_component_param
        = dynamic_cast<EditorEnabledComponent::SetupParam*>(&param);
    assert(editor_enabled_component_param != nullptr && "Invalid setup param type for EditorEnabledComponent");

    // Set parameters


    return true; // Success
}

ecs::ComponentID EditorEnabledComponent::GetID() const
{
    return EditorEnabledComponentHandle::ID();
}

} // namespace mono_entity_archive_extension