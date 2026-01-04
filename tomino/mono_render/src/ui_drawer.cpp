#include "mono_render/src/pch.h"
#include "mono_render/include/ui_drawer.h"

#include "imgui/include/imgui.h"
#include "imgui/include/imgui_internal.h"
#include "imgui/include/imgui_impl_win32.h"
#include "imgui/include/imgui_impl_dx12.h"

namespace mono_render
{

render_graph::ImguiPass::DrawFunc DefaultUIDrawer::CreateDrawFunc()
{
    return [](ImGuiContext* imgui_context) -> bool
    {
        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        ImGui::Begin("Window", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Hello, ImGui!");
        ImGui::End();

        return true; // Success
    };
}

} // namespace mono_render