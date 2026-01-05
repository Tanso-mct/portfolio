#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/menu_bar_drawer.h"

namespace mono_entity_archive_extension
{

void MenuBarDrawer::ResetRequests()
{
    quit_requested_ = false;
    save_requested_ = false;
    project_set_requested_ = false;
    show_hierarchy_requested_ = false;
    show_inspector_requested_ = false;
    show_asset_browser_requested_ = false;
    show_material_editor_requested_ = false;
}

render_graph::ImguiPass::DrawFunc MenuBarDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        ResetRequests();

        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                save_requested_ = ImGui::MenuItem("Save");
                quit_requested_ = ImGui::MenuItem("Quit");

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Show"))
            {
                show_hierarchy_requested_ = ImGui::MenuItem("Hierarchy");
                show_inspector_requested_ = ImGui::MenuItem("Inspector");
                show_asset_browser_requested_ = ImGui::MenuItem("Asset Browser");
                show_material_editor_requested_ = ImGui::MenuItem("Material Editor");
                
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        return true; // Success
    };
}

} // namespace mono_entity_archive_extension