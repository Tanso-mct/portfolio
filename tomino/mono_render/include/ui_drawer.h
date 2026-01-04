#pragma once

#include "render_graph/include/imgui_pass.h"
#include "mono_render/include/dll_config.h"

namespace mono_render
{

// The interface class for UI drawer
class UIDrawer
{
public:
    UIDrawer() = default;
    virtual ~UIDrawer() = default;

    // Create draw function for ImGui pass
    virtual render_graph::ImguiPass::DrawFunc CreateDrawFunc() = 0;
};

class MONO_RENDER_API DefaultUIDrawer : 
    public UIDrawer
{
public:
    DefaultUIDrawer() = default;
    ~DefaultUIDrawer() override = default;

    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;
};

} // namespace mono_render