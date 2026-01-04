#include "mono_render/src/pch.h"
#include "mono_render/include/ui_component.h"

mono_render::UIComponent::UIComponent()
{
}

mono_render::UIComponent::~UIComponent()
{
}

void mono_render::UIComponent::Setup(SetupParam &param)
{
    assert(param.ui_drawer != nullptr && "UI drawer is null in setup parameter");
    ui_drawer_ = std::move(param.ui_drawer);
}

render_graph::ImguiPass::DrawFunc mono_render::UIComponent::CreateDrawFunc()
{
    assert(ui_drawer_ != nullptr && "UI drawer is not set");
    return ui_drawer_->CreateDrawFunc();
}

mono_render::UIDrawer& mono_render::UIComponent::GetDrawer() const
{
    assert(ui_drawer_ != nullptr && "UI drawer is not set");
    return *ui_drawer_;
}

MONO_RENDER_API riaecs::ComponentRegistrar
<mono_render::UIComponent, mono_render::UIComponentMaxCount> mono_render::UIComponentID;