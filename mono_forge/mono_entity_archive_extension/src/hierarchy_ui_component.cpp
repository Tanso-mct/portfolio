#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/hierarchy_ui_component.h"

namespace mono_entity_archive_extension
{

HierarchyUIComponent::HierarchyUIComponent()
{
}

HierarchyUIComponent::~HierarchyUIComponent()
{
}

bool HierarchyUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    HierarchyUIComponent::SetupParam* hierarchy_ui_component_param
        = dynamic_cast<HierarchyUIComponent::SetupParam*>(&param);
    assert(hierarchy_ui_component_param != nullptr && "Invalid setup param type for HierarchyUIComponent");

    // Set parameters


    return true; // Success
}

ecs::ComponentID HierarchyUIComponent::GetID() const
{
    return HierarchyUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension