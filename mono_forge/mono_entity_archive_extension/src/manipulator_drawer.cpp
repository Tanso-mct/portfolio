#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/manipulator_drawer.h"

#include "utility_header/logger.h"

namespace mono_entity_archive_extension
{

render_graph::ImguiPass::DrawFunc ManipulatorDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        ImGui::SetCurrentContext(imgui_context);
        ImGuizmo::SetImGuiContext(imgui_context);

        ImGuizmo::BeginFrame();

        ImGuiIO& io = ImGui::GetIO();
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(window_width, window_height));
        ImGui::Begin(
            "FullScreenGizmoWindow", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize   |
            ImGuiWindowFlags_NoMove     |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoBackground
        );

        ImVec2 win_pos  = ImGui::GetWindowPos();
        ImVec2 win_size = ImGui::GetWindowSize();
        float rect_min_x = win_pos.x;
        float rect_min_y = win_pos.y;
        float rect_max_x = win_pos.x + win_size.x;
        float rect_max_y = win_pos.y + win_size.y;

        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
        ImGuizmo::SetRect(0.0f, 0.0f, window_width, window_height);

        ImGuizmo::Enable(true);
        edited_ = ImGuizmo::Manipulate(
            reinterpret_cast<const float*>(&view_matrix_),
            reinterpret_cast<const float*>(&projection_matrix_),
            current_operation_,
            current_mode_,
            reinterpret_cast<float*>(&object_matrix_));

        ImGui::End();

        return true; // Success
    };
}

} // namespace mono_entity_archive_extension