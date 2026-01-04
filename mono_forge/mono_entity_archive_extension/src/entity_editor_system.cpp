#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/entity_editor_system.h"

#include "ecs/include/world.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_meta_extension/include/meta_component.h"
#include "mono_graphics_extension/include/ui_component.h"
#include "mono_entity_archive_service/include/entity_archive_service_command_list.h"
#include "mono_entity_archive_service/include/entity_archive_service_view.h"
#include "mono_entity_archive_extension/include/editor_enabled_component.h"
#include "mono_entity_archive_extension/include/hierarchy_ui_component.h"
#include "mono_entity_archive_extension/include/inspector_ui_component.h"
#include "mono_entity_archive_extension/include/hierarchy_drawer.h"
#include "mono_entity_archive_extension/include/inspector_drawer.h"

namespace mono_entity_archive_extension
{

// Helper function to get hierarchy ui entities for all scenes
std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> GetHierarchyUIEntities(ecs::World& world)
{
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_id_to_hierarchy_ui_entity;

    // Iterate through all entities with HierarchyUIComponent
    for (const ecs::Entity& entity : world.View(HierarchyUIComponentHandle::ID())())
    {
        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity who has HierarchyUIComponent must have SceneTagComponent");

        // Map scene ID to hierarchy UI entity
        scene_id_to_hierarchy_ui_entity[scene_tag_component->GetSceneID()] = entity;
    }

    return scene_id_to_hierarchy_ui_entity;
}

// Helper function to get editor enable entities for all scenes
std::unordered_map<mono_scene_extension::SceneID, std::vector<ecs::Entity>> GetEditorEnableEntities(ecs::World& world)
{
    std::unordered_map<mono_scene_extension::SceneID, std::vector<ecs::Entity>> scene_id_to_editor_enable_entities;

    // Iterate through all entities with EditorEnabledComponent
    for (const ecs::Entity& entity : world.View(EditorEnabledComponentHandle::ID())())
    {
        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity who has EditorEnabledComponent must have SceneTagComponent");

        // Append entity to the list for the scene ID
        scene_id_to_editor_enable_entities[scene_tag_component->GetSceneID()].emplace_back(entity);
    }

    return scene_id_to_editor_enable_entities;
}

// Helperr function to get inspector ui entities for all scenes
std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> GetInspectorUIEntities(ecs::World& world)
{
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_id_to_inspector_ui_entity;

    // Iterate through all entities with InspectorUIComponent
    for (const ecs::Entity& entity : world.View(InspectorUIComponentHandle::ID())())
    {
        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(scene_tag_component != nullptr && "Entity who has InspectorUIComponent must have SceneTagComponent");

        // Map scene ID to inspector UI entity
        scene_id_to_inspector_ui_entity[scene_tag_component->GetSceneID()] = entity;
    }

    return scene_id_to_inspector_ui_entity;
}

// Helper function to get selected entity from hierarchy drawer
ecs::Entity GetSelectedEntity(HierarchyDrawer* hierarchy_drawer)
{
    // Get selected item index
    int selected_item_index = hierarchy_drawer->GetSelectedItemIndex();

    if (selected_item_index == INVALID_SELECTED_ITEM_INDEX)
    {
        // No item selected
        return ecs::Entity(); // Return invalid entity
    }

    // Get selected entity
    return hierarchy_drawer->GetHierarchyEntities()[selected_item_index];
}

// Helper function to store selected entity for a scene ID
void StoreSelectedEntity(
    HierarchyDrawer* hierarchy_drawer, mono_scene_extension::SceneID scene_id,
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity>& scene_id_to_selected_entity)
{
    // Get selected entity from hierarchy drawer
    ecs::Entity selected_entity = GetSelectedEntity(hierarchy_drawer);

    // Store selected entity for the scene ID
    scene_id_to_selected_entity[scene_id] = selected_entity;
}

// Helper function to update hierarchy drawer with new items and entities
void UpdateHierarchyDrawer(
    HierarchyDrawer* hierarchy_drawer,
    std::vector<ecs::Entity> hierarchy_entities, std::vector<std::string> hierarchy_items)
{
    // Update hierarchy items and entities
    hierarchy_drawer->SetHierarchyItems(std::move(hierarchy_items));
    hierarchy_drawer->SetHierarchyEntities(std::move(hierarchy_entities));
}

void CreateEntityByHierarchy(
    ecs::World& world, size_t scene_id,
    mono_service::ServiceProxy& entity_archive_service_proxy)
{
    // Create a new entity in the world
    ecs::Entity new_entity = world.CreateEntity();
    assert(new_entity.IsValid() && "Failed to create a new entity!");

    // Create entity archive service view
    std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy.CreateView();
    mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view
        = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
    assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

    // Get component adder map
    utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap> component_adder_map
        = entity_archive_service_view->GetComponentAdderMap();

    // Meta component
    {
        // Create setup param for meta component
        std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
            = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();

        // Add meta component to the new entity
        bool result = world.AddComponent<mono_meta_extension::MetaComponent>(
            new_entity, mono_meta_extension::MetaComponentHandle::ID(), std::move(param));

        // Create setup param for meta component again because it was moved
        param = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();

        // Add setup param to entity archive service if editable
        mono_entity_archive_service::AddSetupParam(
            entity_archive_service_proxy, new_entity,
            mono_meta_extension::MetaComponentHandle::ID(), std::move(param));
    }

    // Scene tag component
    {
        // Create setup param for scene tag component
        std::unique_ptr<mono_scene_extension::SceneTagComponent::SetupParam> param
            = std::make_unique<mono_scene_extension::SceneTagComponent::SetupParam>();
        param->scene_id = scene_id;

        bool result = world.AddComponent<mono_scene_extension::SceneTagComponent>(
            new_entity, mono_scene_extension::SceneTagComponentHandle::ID(), std::move(param));
        assert(result);
    }

    // Editor enabled component
    {
        std::unique_ptr<mono_entity_archive_extension::EditorEnabledComponent::SetupParam> param
            = std::make_unique<mono_entity_archive_extension::EditorEnabledComponent::SetupParam>();

        bool result = world.AddComponent<mono_entity_archive_extension::EditorEnabledComponent>(
            new_entity, mono_entity_archive_extension::EditorEnabledComponentHandle::ID(), std::move(param));
        assert(result);
    }
}

EntityEditorSystem::EntityEditorSystem(
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
    mono_service::ServiceProxyManager& service_proxy_manager) :
    entity_archive_service_proxy_(std::move(entity_archive_service_proxy)),
    service_proxy_manager_(service_proxy_manager)
{
    assert(entity_archive_service_proxy_ != nullptr && "Entity archive service proxy is null!");
}

EntityEditorSystem::~EntityEditorSystem()
{
}

bool EntityEditorSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool EntityEditorSystem::Update(ecs::World& world)
{
    // Get hierarchy ui entities for all scenes
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_id_to_hierarchy_ui_entity 
        = GetHierarchyUIEntities(world);
    if (scene_id_to_hierarchy_ui_entity.empty())
        return true; // No hierarchy ui entities, nothing to do

    // Iterate through all scenes with hierarchy ui
    bool any_entity_added = false;
    bool any_entity_deleted = false;
    for (const auto& [scene_id, hierarchy_ui_entity] : scene_id_to_hierarchy_ui_entity)
    {
        // Get UI component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                hierarchy_ui_entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "Entity who has HierarchyUIComponent must have UIComponent");

        // Cast drawer to HierarchyDrawer
        HierarchyDrawer* hierarchy_drawer = dynamic_cast<HierarchyDrawer*>(&ui_component->GetDrawer());
        assert(hierarchy_drawer != nullptr && "UIComponent drawer must be HierarchyDrawer");

        // Check if a new entity has been added
        if (hierarchy_drawer->NewEntityAdded())
        {
            // Create new entity by hierarchy
            CreateEntityByHierarchy(world, scene_id, *entity_archive_service_proxy_);

            // Select the newly added entity in the hierarchy drawer
            hierarchy_drawer->SelectItem(static_cast<int>(hierarchy_drawer->GetHierarchyEntities().size()));

            any_entity_added = true;
        }

        // Check if an entity has been deleted
        ecs::Entity deleted_entity;
        if (hierarchy_drawer->EntityDeleted(deleted_entity))
        {
            // Create entity archive service command list
            std::unique_ptr<mono_service::ServiceCommandList> service_command_list
                = entity_archive_service_proxy_->CreateCommandList();
            mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_service_command_list
                = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(service_command_list.get());
            assert(entity_archive_service_command_list != nullptr && "Entity archive service command list is null!");

            // Erase all setup params for the entity from the entity archive service
            entity_archive_service_command_list->EraseAllSetupParam(deleted_entity);

            // Submit the command list to the entity archive service
            entity_archive_service_proxy_->SubmitCommandList(std::move(service_command_list));

            // Destroy the entity in the world
            world.DestroyEntity(deleted_entity);

            any_entity_deleted = true;
        }
    }

    if (any_entity_added)
        return true; // An entity was added or deleted, skip further processing to avoid issues

    // Get editable entities for all scenes
    std::unordered_map<mono_scene_extension::SceneID, std::vector<ecs::Entity>> scene_id_to_editable_entities 
        = GetEditorEnableEntities(world);

    // Prepare to store selected entities for all scenes
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_id_to_selected_entity;

    // Iterate through all scenes with hierarchy ui
    for (const auto& [scene_id, hierarchy_ui_entity] : scene_id_to_hierarchy_ui_entity)
    {
        // Get editable entities for this scene
        auto editable_entities_it = scene_id_to_editable_entities.find(scene_id);
        if (editable_entities_it == scene_id_to_editable_entities.end())
            continue; // No editable entities for this scene
        const std::vector<ecs::Entity>& editable_entities = editable_entities_it->second;

        // Prepare hierarchy items
        std::vector<ecs::Entity> hierarchy_entities;
        std::vector<std::string> hierarchy_items;

        // Iterate through editable entities to prepare hierarchy items
        for (const ecs::Entity& editable_entity : editable_entities)
        {
            // Get meta component
            mono_meta_extension::MetaComponent* meta_component
                = world.GetComponent<mono_meta_extension::MetaComponent>(
                    editable_entity, mono_meta_extension::MetaComponentHandle::ID());
            assert(meta_component != nullptr && "Entity who has EditorEnabledComponent must have MetaComponent");

            // Append to hierarchy lists
            hierarchy_entities.emplace_back(editable_entity);
            hierarchy_items.emplace_back(std::string(meta_component->GetName()));
        }

        // Get UI component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                hierarchy_ui_entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "Entity who has HierarchyUIComponent must have UIComponent");

        // Cast drawer to HierarchyDrawer
        HierarchyDrawer* hierarchy_drawer = dynamic_cast<HierarchyDrawer*>(&ui_component->GetDrawer());
        assert(hierarchy_drawer != nullptr && "UIComponent drawer must be HierarchyDrawer");

        // Update hierarchy drawer
        UpdateHierarchyDrawer(hierarchy_drawer, std::move(hierarchy_entities),  std::move(hierarchy_items));

        // Store selected entity
        StoreSelectedEntity(hierarchy_drawer, scene_id, scene_id_to_selected_entity);
    }

    /*******************************************************************************************************************
     * Update Inspector
    /******************************************************************************************************************/

    // Get inspector ui component for this scene
    std::unordered_map<mono_scene_extension::SceneID, ecs::Entity> scene_id_to_inspector_ui_entity 
        = GetInspectorUIEntities(world);

    // Iterate through all scenes with inspector ui
    for (const auto& [scene_id, inspector_ui_entity] : scene_id_to_inspector_ui_entity)
    {
        // Find selected entity for this scene
        auto selected_entity_it = scene_id_to_selected_entity.find(scene_id);
        if (selected_entity_it == scene_id_to_selected_entity.end())
            continue; // No selected entity for this scene
        const ecs::Entity& selected_entity = selected_entity_it->second;

        // Get UI component
        mono_graphics_extension::UIComponent* ui_component
            = world.GetComponent<mono_graphics_extension::UIComponent>(
                inspector_ui_entity, mono_graphics_extension::UIComponentHandle::ID());
        assert(ui_component != nullptr && "Entity who has InspectorUIComponent must have UIComponent");

        // Cast drawer to InspectorDrawer
        InspectorDrawer* inspector_drawer = dynamic_cast<InspectorDrawer*>(&ui_component->GetDrawer());
        assert(inspector_drawer != nullptr && "UIComponent drawer must be InspectorDrawer");

        // Check if an entity is selected
        if (!selected_entity.IsValid())
        {
            // No entity selected, clear inspected entity
            inspector_drawer->SetInspectedEntity(ecs::Entity());

            // Clear fields
            inspector_drawer->ClearFields();

            continue; // No entity selected, skip further processing
        }

        if (inspector_drawer->GetInspectedEntity() != selected_entity)
        {
            // Update inspected entity
            inspector_drawer->SetInspectedEntity(selected_entity);

            // Clear previous fields
            inspector_drawer->ClearFields();

            // Get attached components of the selected entity
            utility_header::ConstSharedLockedValue<std::set<ecs::ComponentID>> component_ids
                = world.GetComponentIDs(selected_entity);

            // Create entity archive service view
            std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
            mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view
                = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
            assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

            // Iterate through all attached components
            for (const ecs::ComponentID& component_id : component_ids())
            {
                // Check if the component's setup param is editable
                if (!entity_archive_service_view->CheckIsEditable(selected_entity, component_id))
                    continue; // Not editable, skip

                // Get component field map
                utility_header::ConstSharedLockedValue<component_editor::FieldMap> field_map
                    = entity_archive_service_view->GetComponentFieldMap(component_id);

                // Add fields to inspector drawer
                inspector_drawer->AddField(component_id, field_map());
            }
        }

        // Update field values
        inspector_drawer->UpdateFieldValue();

        // Check for added components
        ecs::ComponentID added_component_id;
        if (inspector_drawer->GetAddedComponentID(added_component_id))
        {
            // Check if the entity already has the component
            if (world.HasComponent(selected_entity, added_component_id))
                continue; // Already has the component, skip

            // Create entity archive service view
            std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
            mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view
                = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
            assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

            // Get component adder map
            utility_header::ConstSharedLockedValue<component_editor::ComponentAdderMap> component_adder_map
                = entity_archive_service_view->GetComponentAdderMap();

            // Find component adder for the added component ID
            auto adder_it = component_adder_map().find(added_component_id);
            assert(adder_it != component_adder_map().end() && "This component does not have a component adder!");
            component_editor::ComponentAdder* component_adder = adder_it->second.get();
            assert(component_adder != nullptr && "Component adder is null!");

            // Add the component to the selected entity using the component adder
            bool result = component_adder->Add(world, selected_entity, service_proxy_manager_);
            assert(result && "Failed to add component using component adder!");

            // Get the component setup param
            std::unique_ptr<ecs::Component::SetupParam> setup_param 
                = component_adder->GetSetupParam(service_proxy_manager_);
            assert(setup_param != nullptr && "Component adder returned null setup param!");

            // Add setup param to entity archive service
            mono_entity_archive_service::AddSetupParam(
                *entity_archive_service_proxy_, selected_entity, added_component_id, std::move(setup_param));

            // Reset inspected entity to refresh fields
            inspector_drawer->SetInspectedEntity(ecs::Entity());
        }

        // Check for removed components
        ecs::ComponentID removed_component_id;
        if (inspector_drawer->GetRemovedComponentID(removed_component_id))
        {
            // Check if the entity has the component
            if (!world.HasComponent(selected_entity, removed_component_id))
                continue; // Does not have the component, skip

            // Remove the component from the selected entity
            bool result = world.RemoveComponent(selected_entity, removed_component_id);
            assert(result && "Failed to remove component from the entity!");

            // Create entity archive service command list
            std::unique_ptr<mono_service::ServiceCommandList> command_list
                = entity_archive_service_proxy_->CreateCommandList();
            mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list
                = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
            assert(entity_archive_command_list != nullptr && "Entity archive service command list is null!");

            // Erase setup param for the component from the entity archive service
            entity_archive_command_list->EraseSetupParam(selected_entity, removed_component_id);

            // Remove fields from inspector drawer
            inspector_drawer->RemoveField(removed_component_id);

            // Submit the command list to the entity archive service
            entity_archive_service_proxy_->SubmitCommandList(std::move(command_list));
        }

        // Reset component added/removed flags
        inspector_drawer->ResetComponentFlag();
    }

    /*******************************************************************************************************************
     * Apply Edited Infos
    /******************************************************************************************************************/

    {
        // Create entity archive service view
        std::unique_ptr<mono_service::ServiceView> service_view = entity_archive_service_proxy_->CreateView();
        mono_entity_archive_service::EntityArchiveServiceView* entity_archive_service_view
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceView*>(service_view.get());
        assert(entity_archive_service_view != nullptr && "Entity archive service view is null!");

        // Get edited infos
        const std::vector<component_editor::EditedInfo>& edited_infos
            = entity_archive_service_view->GetEditedInfos();

        // Check if there are edited infos
        if (edited_infos.empty())
            return true; // No edits, nothing to do

        // Create entity archive service command list
        std::unique_ptr<mono_service::ServiceCommandList> command_list
            = entity_archive_service_proxy_->CreateCommandList();
        mono_entity_archive_service::EntityArchiveServiceCommandList* entity_archive_command_list
            = dynamic_cast<mono_entity_archive_service::EntityArchiveServiceCommandList*>(command_list.get());
        assert(entity_archive_command_list != nullptr && "Entity archive service command list is null!");

        // Iterate through all edited infos
        for (const component_editor::EditedInfo& edited_info : edited_infos)
        {
            // Get setup param from entity archive service view
            utility_header::ConstSharedLockedValue<ecs::Component::SetupParam> setup_param
                = entity_archive_service_view->GetSetupParam(edited_info.entity, edited_info.component_id);

            // Get component
            ecs::Component* component
                = world.GetComponent<ecs::Component>(edited_info.entity, edited_info.component_id);
            assert(component != nullptr && "Edited entity must have the edited component");

            // Apply changes to the component
            if (!component->Apply(setup_param()))
                assert(false && "Failed to apply changes to the component");
        }
    }

    return true; // Success
}

bool EntityEditorSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID EntityEditorSystem::GetID() const
{
    return EntityEditorSystemHandle::ID();
}

} // namespace mono_entity_archive_extension