#include "mono_graphics_extension/src/pch.h"
#include "mono_graphics_extension/include/ui_component.h"

namespace mono_graphics_extension
{

UIComponent::UIComponent()
{
}

UIComponent::~UIComponent()
{
}

bool UIComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    UIComponent::SetupParam* ui_component_param
        = dynamic_cast<UIComponent::SetupParam*>(&param);
    assert(ui_component_param != nullptr && "Invalid setup param type for UIComponent");

    // Set parameters
    ui_drawer_ = std::move(ui_component_param->ui_drawer);

    return true; // Success
}

ecs::ComponentID UIComponent::GetID() const
{
    return UIComponentHandle::ID();
}

render_graph::ImguiPass::DrawFunc UIComponent::CreateDrawFunc()
{
    assert(ui_drawer_ != nullptr && "UI drawer is null");
    return ui_drawer_->CreateDrawFunc();
}

UIDrawer& UIComponent::GetDrawer() const
{
    assert(ui_drawer_ != nullptr && "UI drawer is null");
    return *ui_drawer_;
}

} // namespace mono_graphics_extension