#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/hierarchy_drawer.h"

namespace mono_entity_archive_extension
{

render_graph::ImguiPass::DrawFunc HierarchyDrawer::CreateDrawFunc()
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
        ImVec2 winPos = ImVec2(window_width * HIERARCHY_WINDOW_POS_X_RATIO, window_height * HIERARCHY_WINDOW_POS_Y_RATIO);
        ImGui::SetNextWindowPos(winPos, ImGuiCond_Once);

        // Set new window size
        ImVec2 winSize = ImVec2(window_width * HIERARCHY_WINDOW_WIDTH_RATIO, window_height * HIERARCHY_WINDOW_HEIGHT_RATIO);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Once);

        // Create a window for the hierarchy
        ImGui::Begin("Hierarchy", &show_window_);

        // If the window is hovered and right mouse button is clicked
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup) &&
            !ImGui::IsAnyItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            // Set the position for the popup and open it
            ImGui::SetNextWindowPos(io.MouseClickedPos[ImGuiMouseButton_Right]);
            ImGui::OpenPopup("HierarchyContextPopup");
        }

        // Popup for context menu
        if (ImGui::BeginPopup("HierarchyContextPopup"))
        {
            if (ImGui::MenuItem("Add Entity"))
            {
                // Mark that a new entity has been added
                new_entity_added_ = true;
            }
            
            ImGui::EndPopup();
        }

        // Display hierarchy items
        for (size_t i = 0; i < hierarchy_items_.size(); ++i)
        {
            // Get the item
            const std::string& item = hierarchy_items_[i];

            // Store currently displaying items
            displaying_hierarchy_items_[i] = item + "##item" + std::to_string(i);

            if (ImGui::Selectable(displaying_hierarchy_items_[i].c_str(), selected_item_index_ == static_cast<int>(i)))
            {
                // Update selected item index
                selected_item_index_ = static_cast<int>(i);
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Delete"))
                {
                    // Mark that an entity has been deleted
                    entity_deleted_ = true;

                    // Store the deleted entity
                    assert(i < hierarchy_entities_.size() && "Index out of bounds when deleting entity from hierarchy.");
                    deleted_entity_ = hierarchy_entities_[i];

                    // If the deleted entity was selected, reset the selection
                    if (selected_item_index_ == static_cast<int>(i))
                        selected_item_index_ = INVALID_SELECTED_ITEM_INDEX;
                }
                ImGui::EndPopup();
            }
        }

        ImGui::End();

        return true; // Success
    };
}

void HierarchyDrawer::SetHierarchyItems(std::vector<std::string> items)
{
    // Update hierarchy items
    hierarchy_items_ = std::move(items);

    // Resize displaying items vector if needed
    if (displaying_hierarchy_items_.size() != hierarchy_items_.size())
        displaying_hierarchy_items_.resize(hierarchy_items_.size());
}

bool HierarchyDrawer::NewEntityAdded()
{
    bool last_state = new_entity_added_;

    // Reset the flag after checking
    new_entity_added_ = false;

    return last_state; // Return whether a new entity was added since last check
}

bool HierarchyDrawer::EntityDeleted(ecs::Entity& out_delete_entity)
{
    bool last_state = entity_deleted_;
    out_delete_entity = deleted_entity_;

    // Reset the flag after checking
    entity_deleted_ = false;
    deleted_entity_ = ecs::Entity();

    return last_state; // Return whether an entity was deleted since last check
}

} // namespace mono_entity_archive_extension