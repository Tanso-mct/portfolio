#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/material_editor_drawer.h"

#include "render_graph/include/material_handle_manager.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace mono_entity_archive_extension
{

MaterialEditorDrawer::MaterialEditorDrawer(
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy,
    mono_service::ServiceProxyManager& service_proxy_manager) :
    entity_archive_service_proxy_(std::move(entity_archive_service_proxy)),
    graphics_service_proxy_(std::move(graphics_service_proxy)),
    service_proxy_manager_(service_proxy_manager)
{
    assert(entity_archive_service_proxy_ != nullptr && "Entity archive service proxy is null!");
    assert(graphics_service_proxy_ != nullptr && "Graphics service proxy is null!");
}

std::vector<MaterialEditorDrawer::EditedMaterialInfo> MaterialEditorDrawer::TakeEditedMaterialInfos()
{
    std::vector<EditedMaterialInfo> taken_infos = std::move(edited_material_infos_);
    edited_material_infos_.clear();
    return taken_infos;
}

render_graph::ImguiPass::DrawFunc MaterialEditorDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        // Reset material creation request
        create_material_requested_ = MaterialCreateRequest();
        delete_material_requested_ = MaterialDeleteRequest();

        if (!show_window_)
        {
            selected_ = false;
            last_selected_index_ = 0;
            return true;
        }

        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        // Get IO object
        ImGuiIO& io = ImGui::GetIO();

        // Get window size
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;

        // Set new window position
        ImVec2 winPos = ImVec2(window_width * MATERIAL_EDITOR_WINDOW_POS_X_RATIO, window_height * MATERIAL_EDITOR_WINDOW_POS_Y_RATIO);
        ImGui::SetNextWindowPos(winPos, ImGuiCond_Once);

        // Set new window size
        ImVec2 winSize = ImVec2(window_width * MATERIAL_EDITOR_WINDOW_WIDTH_RATIO, window_height * MATERIAL_EDITOR_WINDOW_HEIGHT_RATIO);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Once);

        // Begin the material editor window
        ImGui::Begin("MaterialEditor", &show_window_);

        // Get registered material handle IDs
        std::vector<uint32_t> material_handle_keys
            = render_graph::MaterialHandleManager::GetInstance().GetRegisteredMaterialHandleKeys();

        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> entity_archive_service_view = entity_archive_service_proxy_->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(entity_archive_service_view.get());
        assert(entity_archive_service_view_ptr != nullptr && "Failed to create entity archive service view!");

        // Create graphics service view
        std::unique_ptr<mono_service::ServiceView> graphics_service_view = graphics_service_proxy_->CreateView();
        mono_graphics_service::GraphicsServiceView* graphics_service_view_ptr
            = dynamic_cast<mono_graphics_service::GraphicsServiceView*>(graphics_service_view.get());
        assert(graphics_service_view_ptr != nullptr && "Failed to create graphics service view!");

        if (ImGui::BeginTable("MaterialTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders))
        {
            ImGui::TableNextColumn();

            // Display each material with its name
            for (size_t i = 0; i < material_handle_keys.size(); ++i)
            {
                // Get material handle key
                render_graph::MaterialHandleKey material_handle_key = material_handle_keys[i];

                // Get material name
                std::string_view material_name
                    = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandleName(material_handle_key);

                // Push ID for ImGui
                ImGui::PushID(i);

                // Display material name
                if (ImGui::Selectable(std::string(material_name).c_str()))
                {
                    selected_ = true;
                    last_selected_index_ = i;
                }

                if (renaming_ && renaming_index_ == i)
                {
                    if (request_focus_input_)
                    {
                        ImGui::SetKeyboardFocusHere();
                        request_focus_input_ = false;
                    }

                    // Move to same line
                    ImGui::SameLine();

                    if (ImGui::InputText(
                        "##RenameMaterialInput", rename_buffer_, sizeof(rename_buffer_), 
                        ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        // Set new material name
                        render_graph::MaterialHandleManager::GetInstance().SetMaterialHandleName(
                            material_handle_key, std::string_view(rename_buffer_));

                        renaming_ = false;
                    }
                }

                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Rename"))
                    {
                        renaming_ = true;
                        renaming_index_ = i;
                        request_focus_input_ = true;

                        // Get current material handle name
                        std::string_view current_name
                            = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandleName(material_handle_key);

                        // Copy current name to rename buffer
                        std::strncpy(rename_buffer_, current_name.data(), sizeof(rename_buffer_));
                    }

                    if (ImGui::MenuItem("Delete"))
                    {
                        delete_material_requested_.requested = true;
                        delete_material_requested_.material_handle_key = material_handle_key;
						selected_ = false;
                    }

                    ImGui::EndPopup();
                }

                // Pop ID for ImGui
                ImGui::PopID();
            }

            ImGui::TableNextColumn();

            // Display each material with its name
            if (selected_)
            {
                // Get material handle key
                render_graph::MaterialHandleKey material_handle_key = material_handle_keys[last_selected_index_];

                // Get material handle
                render_graph::MaterialHandle* material_handle
                    = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandle(material_handle_key);

                // Get material type handle ID
                render_graph::MaterialTypeHandleID material_type_handle_id
                    = graphics_service_view_ptr->GetMaterialType(material_handle);

                // Get material setup param edit function
                const mono_entity_archive_service::MaterialSetupParamEditFunc& setup_param_edit_func
                    = entity_archive_service_view_ptr->GetMaterialSetupParamEditFunc(material_type_handle_id);
                assert(setup_param_edit_func && "Material setup param edit function is null!");

                // Get current material setup param
                const material_editor::SetupParamWrapper* material_setup_param
                    = entity_archive_service_view_ptr->GetMaterialSetupParam(material_handle);
                assert(material_setup_param != nullptr && "Material setup param is null!");

                // Edit material setup param using the edit function
                std::unique_ptr<material_editor::SetupParamWrapper> edited_setup_param
                    = setup_param_edit_func(material_setup_param, service_proxy_manager_);

                if (edited_setup_param != nullptr)
                {
                    // Create edited material info
                    EditedMaterialInfo edited_info;
                    edited_info.material_handle_key = material_handle_key;
                    edited_info.material_handle = material_handle;
                    edited_info.setup_param = std::move(edited_setup_param);

                    // Store the edited material info
                    edited_material_infos_.emplace_back(std::move(edited_info));
                }
            }

            ImGui::EndTable();
        }

        if (
            ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            ImGui::SetNextWindowPos(io.MouseClickedPos[ImGuiMouseButton_Right]);
            ImGui::OpenPopup("MaterialEditorContextPopup");
        }

        if (ImGui::BeginPopup("MaterialEditorContextPopup"))
        {
            if (ImGui::BeginMenu("Create Material"))
            {
                // Get all registered material type handle IDs from entity archive service view
                std::vector<render_graph::MaterialTypeHandleID> material_type_handle_ids
                    = entity_archive_service_view_ptr->GetMaterialSetupParamEditorRegistry().GetRegisteredMaterialTypeHandleIDs();
                assert(!material_type_handle_ids.empty() && "No registered material type handle IDs found!");

                for (const auto& material_type_handle_id : material_type_handle_ids)
                {
                    // Get material type name
                    const std::string& material_type_name
                        = entity_archive_service_view_ptr->GetMaterialSetupParamEditorRegistry().GetMaterialTypeName(material_type_handle_id);

                    // Create menu item for each material type
                    if (ImGui::MenuItem(material_type_name.c_str()))
                    {
                        create_material_requested_.requested = true;
                        create_material_requested_.material_type_handle_id = material_type_handle_id;
                    }
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        ImGui::End();

        return true; // Success
    };
}

} // namespace mono_entity_archive_extension