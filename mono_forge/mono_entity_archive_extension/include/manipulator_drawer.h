#pragma once

#include <DirectXMath.h>
#include "imgui/include/imgui.h"
#include "imgui/include/ImGuizmo.h"

#include "ecs/include/entity.h"
#include "mono_graphics_extension/include/ui_drawer.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

class MONO_ENTITY_ARCHIVE_EXT_DLL ManipulatorDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    ManipulatorDrawer() = default;
    ~ManipulatorDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    // Set the view matrix
    void SetViewMatrix(const DirectX::XMFLOAT4X4& view_matrix) { view_matrix_ = view_matrix; }

    // Set the projection matrix
    void SetProjectionMatrix(const DirectX::XMFLOAT4X4& projection_matrix) { projection_matrix_ = projection_matrix; }

    // Set the object matrix
    void SetObjectMatrix(const DirectX::XMFLOAT4X4& object_matrix) { object_matrix_ = object_matrix; }

    // Get the object matrix
    DirectX::XMFLOAT4X4 GetObjectMatrix() const { return object_matrix_; }

    // Set editing entity
    void SetEditingEntity(const ecs::Entity& entity) { editing_entity_ = entity; }

    // Get editing entity
    ecs::Entity GetEditingEntity() const { return editing_entity_; }

    // Whether the manipulator is being edited in the current frame
    bool IsEdited() const { return edited_; }

private:
    // View matrix for the manipulator
    DirectX::XMFLOAT4X4 view_matrix_;

    // Projection matrix for the manipulator
    DirectX::XMFLOAT4X4 projection_matrix_;

    // Object matrix for the manipulator
    DirectX::XMFLOAT4X4 object_matrix_;

    // The entity currently being edited
    ecs::Entity editing_entity_ = ecs::Entity();

    // Whether the manipulator is edited in the current frame
    bool edited_ = false;

    // Current operation mode for the manipulator
    ImGuizmo::OPERATION current_operation_ = ImGuizmo::TRANSLATE;

    // Current mode for the manipulator (local or world)
    ImGuizmo::MODE current_mode_ = ImGuizmo::WORLD;
};

} // namespace mono_entity_archive_extension
