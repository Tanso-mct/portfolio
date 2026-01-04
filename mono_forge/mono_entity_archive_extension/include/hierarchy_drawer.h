#pragma once

#include "ecs/include/entity.h"
#include "mono_graphics_extension/include/ui_drawer.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

constexpr int INVALID_SELECTED_ITEM_INDEX = -1;

constexpr float HIERARCHY_WINDOW_WIDTH_RATIO = 0.2f;
constexpr float HIERARCHY_WINDOW_HEIGHT_RATIO = 1.0f;

constexpr float HIERARCHY_WINDOW_POS_X_RATIO = 0.0f;
constexpr float HIERARCHY_WINDOW_POS_Y_RATIO = 0.025f;

class MONO_ENTITY_ARCHIVE_EXT_DLL HierarchyDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    HierarchyDrawer() = default;
    ~HierarchyDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    // Set the hierarchy items to display
    void SetHierarchyItems(std::vector<std::string> items);

    // Get the hierarchy entities
    const std::vector<ecs::Entity>& GetHierarchyEntities() const { return hierarchy_entities_; }

    // Set the corresponding entities for the hierarchy items
    void SetHierarchyEntities(std::vector<ecs::Entity> entities) { hierarchy_entities_ = std::move(entities); }

    // Get the index of the currently selected item
    int GetSelectedItemIndex() const { return selected_item_index_; }

    // Check if the new entity has been added since last check
    bool NewEntityAdded();

    // Check if an entity has been deleted since last check
    bool EntityDeleted(ecs::Entity& out_delete_entity);

    // Select an item by index
    void SelectItem(int index) { selected_item_index_ = index; }

private:
    // List of hierarchy items to display
    std::vector<std::string> hierarchy_items_;

    // Currently displaying hierarchy items
    std::vector<std::string> displaying_hierarchy_items_;

    // Corresponding entities for the hierarchy items
    std::vector<ecs::Entity> hierarchy_entities_;

    // Index of the currently selected item
    int selected_item_index_ = INVALID_SELECTED_ITEM_INDEX;

    // Whether the hierarchy window is open
    bool is_opend_ = false;

    // Whether a new entity has been added since last check
    bool new_entity_added_ = false;

    // Whether an entity has been deleted since last check
    bool entity_deleted_ = false;

    // The last deleted entity
    ecs::Entity deleted_entity_ = ecs::Entity();
};

} // namespace mono_entity_archive_extension
