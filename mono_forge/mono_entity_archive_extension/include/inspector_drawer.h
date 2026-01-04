#pragma once

#include <any>

#include "ecs/include/entity.h"
#include "ecs/include/component.h"
#include "component_editor/include/component_reflection_info.h"
#include "mono_graphics_extension/include/ui_drawer.h"

#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// Structure to hold field information
struct FieldInfo
{
    // Field name
    std::string name;

    // Field type name
    std::string type_name;

    // Field value
    std::any value;

    // Equality operator for FieldInfo
    bool operator==(const FieldInfo& other) const
    {
        return name == other.name;
    }
};

} // namespace mono_entity_archive_extension

namespace std
{
    // Specialization of std::hash for FieldInfo
    // If name is the same, consider it the same field
    template <>
    struct hash<mono_entity_archive_extension::FieldInfo>
    {
        std::size_t operator()(const mono_entity_archive_extension::FieldInfo& field_info) const
        {
            return std::hash<std::string>()(field_info.name);
        }
    };
} // namespace std

namespace mono_entity_archive_extension
{

constexpr float INSPECTOR_WINDOW_WIDTH_RATIO = 0.3f;
constexpr float INSPECTOR_WINDOW_HEIGHT_RATIO = 1.0f;

constexpr float INSPECTOR_WINDOW_POS_X_RATIO = 0.7f;
constexpr float INSPECTOR_WINDOW_POS_Y_RATIO = 0.025f;

class MONO_ENTITY_ARCHIVE_EXT_DLL InspectorDrawer : 
    public mono_graphics_extension::UIDrawer
{
public:
    InspectorDrawer(
        mono_service::ServiceProxyManager& service_proxy_manager,
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy) :
        service_proxy_manager_(service_proxy_manager),
        entity_archive_service_proxy_(std::move(entity_archive_service_proxy))
    {
    }
    
    ~InspectorDrawer() override = default;
    render_graph::ImguiPass::DrawFunc CreateDrawFunc() override;

    // Set the inspected entity
    void SetInspectedEntity(const ecs::Entity& entity) { inspected_entity_ = entity; }

    // Get the inspected entity
    const ecs::Entity& GetInspectedEntity() const { return inspected_entity_; }

    // Add fields
    void AddField(ecs::ComponentID component_id, const component_editor::FieldMap& field_map);

    // Update field values from the service
    void UpdateFieldValue();

    // Remove fields for a component ID
    void RemoveField(ecs::ComponentID component_id);

    // Clear fields
    void ClearFields() { component_id_to_field_infos_.clear(); }

    // Check if a new component ID has been added
    bool GetAddedComponentID(ecs::ComponentID& out_component_id) const;

    // Check if a component ID has been removed
    bool GetRemovedComponentID(ecs::ComponentID& out_component_id) const;

    // Reset component added/removed flags
    void ResetComponentFlag();

private:
    // Reference to the service proxy manager
    mono_service::ServiceProxyManager& service_proxy_manager_;

    // Entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_ = nullptr;

    // Map from component ID to its field infos
    std::unordered_map<ecs::ComponentID, std::vector<FieldInfo>> component_id_to_field_infos_;

    // Currently inspected entity
    ecs::Entity inspected_entity_ = ecs::Entity();

    // Added component IDs since last frame
    ecs::ComponentID added_component_id_ = ecs::ComponentID();

    // Whether a component ID was added since last frame
    bool component_id_added_ = false;

    // Removed component IDs since last frame
    ecs::ComponentID removed_component_id_ = ecs::ComponentID();

    // Whether a component ID was removed since last frame
    bool component_id_removed_ = false;

    // Buffer for component search input
    char component_search_buf[128] = "";
};

} // namespace mono_entity_archive_extension
