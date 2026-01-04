#pragma once

#include "render_graph/include/imgui_pass.h"
#include "mono_graphics_extension/include/dll_config.h"

namespace mono_graphics_extension
{

// The interface class for UI drawer
class UIDrawer
{
public:
    UIDrawer() = default;
    virtual ~UIDrawer() = default;

    // Create draw function for ImGui pass
    virtual render_graph::ImguiPass::DrawFunc CreateDrawFunc() = 0;

    // Show or hide the UI
    void SetShowWindow(bool show) { show_window_ = show; }

protected:
    bool show_window_ = true;
};

class MONO_GRAPHICS_EXT_DLL DefaultUIDrawer : 
    public UIDrawer
{
public:
    DefaultUIDrawer() = default;
    ~DefaultUIDrawer() override = default;

    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;
};

} // namespace mono_graphics_extension