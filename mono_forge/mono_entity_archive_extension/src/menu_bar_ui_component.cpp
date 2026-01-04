#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/menu_bar_ui_component.h"

namespace mono_entity_archive_extension
{

MenuBarUIComponent::MenuBarUIComponent()
{
}

MenuBarUIComponent::~MenuBarUIComponent()
{
}

bool MenuBarUIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    MenuBarUIComponent::SetupParam* menu_bar_ui_component_param
        = dynamic_cast<MenuBarUIComponent::SetupParam*>(&param);
    assert(menu_bar_ui_component_param != nullptr && "Invalid setup param type for MenuBarUIComponent");

    // Set parameters
    hierarchy_entity_ = menu_bar_ui_component_param->hierarchy_entity_;
    inspector_entity_ = menu_bar_ui_component_param->inspector_entity_;
    asset_browser_entity_ = menu_bar_ui_component_param->asset_browser_entity_;
    material_editor_entity_ = menu_bar_ui_component_param->material_editor_entity_;

    return true; // Success
}

bool MenuBarUIComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const MenuBarUIComponent::SetupParam* menu_bar_ui_component_param
        = dynamic_cast<const MenuBarUIComponent::SetupParam*>(&param);
    assert(menu_bar_ui_component_param != nullptr && "Invalid setup param type for MenuBarUIComponent");

    // Set parameters
    hierarchy_entity_ = menu_bar_ui_component_param->hierarchy_entity_;
    inspector_entity_ = menu_bar_ui_component_param->inspector_entity_;
    asset_browser_entity_ = menu_bar_ui_component_param->asset_browser_entity_;
    material_editor_entity_ = menu_bar_ui_component_param->material_editor_entity_;

    return true; // Success
}

ecs::ComponentID MenuBarUIComponent::GetID() const
{
    return MenuBarUIComponentHandle::ID();
}

} // namespace mono_entity_archive_extension