#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/inspector_drawer.h"

#include "mono_entity_archive_service/include/entity_archive_service.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"

namespace mono_entity_archive_extension
{

render_graph::ImguiPass::DrawFunc InspectorDrawer::CreateDrawFunc()
{
    return [this](ImGuiContext* imgui_context) -> bool
    {
        if (!show_window_)
            return true; // Success

        // Set current ImGui context
        ImGui::SetCurrentContext(imgui_context);

        // Get IO object
        ImGuiIO& io = ImGui::GetIO();

        // Get window size
        float window_width = io.DisplaySize.x;
        float window_height = io.DisplaySize.y;

        // Set new window position
        ImVec2 winPos = ImVec2(window_width * INSPECTOR_WINDOW_POS_X_RATIO, window_height * INSPECTOR_WINDOW_POS_Y_RATIO);
        ImGui::SetNextWindowPos(winPos, ImGuiCond_Once);

        // Set new window size
        ImVec2 winSize = ImVec2(window_width * INSPECTOR_WINDOW_WIDTH_RATIO, window_height * INSPECTOR_WINDOW_HEIGHT_RATIO);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Once);

        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* service_view_ptr
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
        assert(service_view_ptr != nullptr && "Entity archive service view is null!");

        // Create a window for the inspector
        ImGui::Begin("Inspector", &show_window_);

        // If the window is hovered and right mouse button is clicked
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            // Set the position for the popup and open it
            ImGui::SetNextWindowPos(io.MouseClickedPos[ImGuiMouseButton_Right]);
            ImGui::OpenPopup("InspectorContextPopup");
        }

        // Popup for context menu
        if (ImGui::BeginPopup("InspectorContextPopup"))
        {
            if (ImGui::BeginMenu("Add Component"))
            {
                // Input text for component search
                ImGui::InputText("Search", component_search_buf, sizeof(component_search_buf));

                // Convert search string to std::string
                std::string search_str(component_search_buf);

                // Get component name map
                utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap> component_name_map
                    = service_view_ptr->GetComponentNameMap();

                for (const auto& [component_id, component_name] : component_name_map())
                {
                    // If the component is editable
                    if (service_view_ptr->CheckComponentEditable(component_id))
                    {
                        if (!search_str.empty()) // Skip if search string is empty
                        {
                            // If the component name does not contain the search string, skip it
                            if (component_name.find(search_str) == std::string::npos)
                                continue;
                        }

                        // If the menu item is selected
                        if (ImGui::MenuItem(component_name.c_str()))
                        {
                            // Mark that a new component ID has been added
                            added_component_id_ = component_id;
                            component_id_added_ = true;
                        }
                    }
                }

                ImGui::EndMenu();
            }
            
            ImGui::EndPopup();
        }

        // Iterate over each component's field infos
        for (auto& [component_id, field_infos] : component_id_to_field_infos_)
        {
            // Hold component name
            std::string_view component_name;
            {
                // Get component name map
                utility_header::ConstSharedLockedValue<component_editor::ComponentNameMap> component_name_map
                    = service_view_ptr->GetComponentNameMap();

                // Get component name
                auto name_it = component_name_map().find(component_id);
                assert(name_it != component_name_map().end() && "Component ID not found in component name map.");
                component_name = name_it->second;
            }

            // Set the tree node to be open by default
            ImGui::SetNextItemOpen(true, ImGuiCond_Once);

            // Create a tree node for the component
            if (ImGui::TreeNode(component_name.data()))
            {
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Remove"))
                    {
                        // Mark that a component ID has been removed
                        removed_component_id_ = component_id;
                        component_id_removed_ = true;
                    }

                    ImGui::EndPopup();
                }

                // Display each field info
                for (FieldInfo& field_info : field_infos)
                {
                    // Get field value edit function
                    const mono_entity_archive_service::ComponentSetupParamAnyFieldEditFunc& edit_func
                        = service_view_ptr->GetSetupParamFieldTypeRegistry()->GetSetupParamFieldEditFunc(field_info.type_name);

                    // Edit the field value
                    bool edited = edit_func(field_info.value, field_info.name, service_proxy_manager_);

                    if (edited)
                    {
                        // Create entity archive service command list
                        std::unique_ptr<mono_service::ServiceCommandList> command_list
                            = entity_archive_service_proxy_->CreateCommandList();
                        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list
                            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
                        assert(entity_archive_command_list != nullptr && "Entity archive command list is null!");

                        // Edit setup param field in entity archive service
                        entity_archive_command_list->EditSetupParamField(
                            inspected_entity_, component_id, field_info.name, field_info.value);

                        // Submit command list
                        entity_archive_service_proxy_->SubmitCommandList(std::move(command_list));
                    }
                }

                // Pop the tree node after finishing displaying fields
                ImGui::TreePop();
            }
        }

        // End the inspector window
        ImGui::End();

        return true; // Success
    };
}

void InspectorDrawer::AddField(
    ecs::ComponentID component_id, const component_editor::FieldMap& field_map)
{
    // Vector to hold field infos
    std::vector<FieldInfo> field_infos;

    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
    mono_entity_archive_service::EntityArchiveServiceView* service_view_ptr
        = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
    assert(service_view_ptr != nullptr && "Entity archive service view is null!");

    for (const auto& [field_name, field_info] : field_map)
    {
        FieldInfo info;

        // Set field name
        info.name = field_name;

        // Set field type name
        info.type_name = field_info.type_name;

        // Add to field infos vector
        field_infos.emplace_back(std::move(info));
    }

    // Store field infos in the map
    component_id_to_field_infos_[component_id] = std::move(field_infos);
}

void InspectorDrawer::UpdateFieldValue()
{
    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
    mono_entity_archive_service::EntityArchiveServiceView* service_view_ptr
        = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
    assert(service_view_ptr != nullptr && "Entity archive service view is null!");

    for (auto& [component_id, field_infos] : component_id_to_field_infos_)
    {
        for (FieldInfo& field_info : field_infos)
        {
            // Get updated field value
            const uint8_t* field_value = service_view_ptr->GetSetupParamField(
                inspected_entity_, component_id, field_info.name);

            // Get setup param field create function
            const mono_entity_archive_service::ComponentSetupParamAnyFieldCreateFunc& create_func
                = service_view_ptr->GetSetupParamFieldTypeRegistry()->GetSetupParamFieldCreateFunc(field_info.type_name);

            // Update field value as std::any
            field_info.value = create_func(field_value, service_proxy_manager_);
        }
    }
}

void InspectorDrawer::RemoveField(ecs::ComponentID component_id)
{
    assert(
        component_id_to_field_infos_.find(component_id) != component_id_to_field_infos_.end()
        && "Component ID not found in InspectorDrawer field infos.");
    component_id_to_field_infos_.erase(component_id);
}

bool InspectorDrawer::GetAddedComponentID(ecs::ComponentID& out_component_id) const
{
    if (component_id_added_)
    {
        out_component_id = added_component_id_;
        return true;
    }

    return false;
}

bool InspectorDrawer::GetRemovedComponentID(ecs::ComponentID& out_component_id) const
{
    if (component_id_removed_)
    {
        out_component_id = removed_component_id_;
        return true;
    }

    return false;
}

void InspectorDrawer::ResetComponentFlag()
{
    component_id_added_ = false;
    component_id_removed_ = false;
}

} // namespace mono_entity_archive_extension