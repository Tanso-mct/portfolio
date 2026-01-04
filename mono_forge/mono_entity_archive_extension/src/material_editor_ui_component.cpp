#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/material_editor_ui_component.h"

namespace mono_entity_archive_extension
{

MaterialEditorUIComponent::MaterialEditorUIComponent()
{
}

MaterialEditorUIComponent::~MaterialEditorUIComponent()
{
}

bool MaterialEditorUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    MaterialEditorUIComponent::SetupParam* material_editor_ui_component_param
        = dynamic_cast<MaterialEditorUIComponent::SetupParam*>(&param);
    assert(material_editor_ui_component_param != nullptr && "Invalid setup param type for MaterialEditorUIComponent");

    // Set parameters


    return true; // Success
}

bool MaterialEditorUIComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const MaterialEditorUIComponent::SetupParam* material_editor_ui_component_param
        = dynamic_cast<const MaterialEditorUIComponent::SetupParam*>(&param);
    assert(material_editor_ui_component_param != nullptr && "Invalid setup param type for MaterialEditorUIComponent");

    // Set parameters

    return true; // Success
}

ecs::ComponentID MaterialEditorUIComponent::GetID() const
{
    return MaterialEditorUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension