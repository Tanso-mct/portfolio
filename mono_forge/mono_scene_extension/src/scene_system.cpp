#include "mono_scene_extension/src/pch.h"
#include "mono_scene_extension/include/scene_system.h"

#include "ecs/include/world.h"
#include "mono_meta_extension/include/meta_component.h"
#include "mono_scene_extension/include/scene_tag_component.h"
#include "mono_scene_extension/include/scene_component.h"

namespace mono_scene_extension
{

// Helper function to get entities for a scene based on required component IDs
std::vector<ecs::Entity> GetEntitiesForScene(
    ecs::World& world, const std::vector<ecs::ComponentID>& required_component_ids, SceneID scene_id)
{
    // Create list of entities
    std::vector<ecs::Entity> entities;

    // Select component id for iteration
    ecs::ComponentID selected_component_id = required_component_ids.front();

    // Iterate through all entities who has selected component id
    for (const ecs::Entity& selected_entity : world.View(selected_component_id)())
    {
        // Get meta component
        mono_meta_extension::MetaComponent* meta_component
            = world.GetComponent<mono_meta_extension::MetaComponent>(
                selected_entity, mono_meta_extension::MetaComponentHandle::ID());
        assert(meta_component != nullptr && "Entity who has window associated component must have meta component!");

        if (!meta_component->IsActiveSelf())
            continue; // Skip if the entity is not active

        // Get scene tag component
        mono_scene_extension::SceneTagComponent* scene_tag_component
            = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                selected_entity, mono_scene_extension::SceneTagComponentHandle::ID());
        assert(
            scene_tag_component != nullptr && 
            "Entity who has window associated component must have scene tag component!");
            
        if (scene_tag_component->GetSceneID() != scene_id)
            continue; // Skip if the scene ID does not match

        // Check if the entity has all required component ids
        bool has_all_required_components = true;
        for (const ecs::ComponentID& required_component_id : required_component_ids)
        {
            if (!world.HasComponent(selected_entity, required_component_id))
            {
                has_all_required_components = false;
                break;
            }
        }

        if (!has_all_required_components)
            continue; // Skip if not all required components are present

        // Add the selected entity to the list
        entities.push_back(selected_entity);
    }

    return entities; // Return the list of window entities
}

std::vector<std::pair<ecs::ComponentID, ecs::Entity>> GetEntitiesForSceneWhoHasAnyComponent(
    ecs::World& world, const std::vector<ecs::ComponentID>& component_ids, SceneID scene_id)
{
    // Create map of entities to avoid duplicates
    std::unordered_map<ecs::ComponentID, std::unordered_set<ecs::Entity>> entity_map;

    for (const ecs::ComponentID& component_id : component_ids)
    {
        // Iterate through all entities who has selected component id
        for (const ecs::Entity& entity : world.View(component_id)())
        {
            // Get meta component
            mono_meta_extension::MetaComponent* meta_component
                = world.GetComponent<mono_meta_extension::MetaComponent>(
                    entity, mono_meta_extension::MetaComponentHandle::ID());
            assert(meta_component != nullptr && "Entity who has window associated component must have meta component!");

            if (!meta_component->IsActiveSelf())
                continue; // Skip if the entity is not active

            // Get scene tag component
            mono_scene_extension::SceneTagComponent* scene_tag_component
                = world.GetComponent<mono_scene_extension::SceneTagComponent>(
                    entity, mono_scene_extension::SceneTagComponentHandle::ID());
            assert(
                scene_tag_component != nullptr && 
                "Entity who has window associated component must have scene tag component!");
                
            if (scene_tag_component->GetSceneID() != scene_id)
                continue; // Skip if the scene ID does not match

            // Add the selected entity to the list if not already present
            entity_map[component_id].insert(entity);
        }
    }

    std::vector<std::pair<ecs::ComponentID, ecs::Entity>> entity_list;
    for (const auto& [component_id, entities] : entity_map)
        for (const ecs::Entity& entity : entities)
            entity_list.emplace_back(component_id, entity);
            
    return entity_list; // Return the list of window entities
}

SceneSystem::SceneSystem()
{
    // Create singleton instance of SceneIDGenerator
    scene_id_generator_ = std::make_unique<SceneIDGenerator>();
}

SceneSystem::~SceneSystem()
{
    // Cleanup
    scene_id_generator_.reset();
}

bool SceneSystem::PreUpdate(ecs::World& world)
{
    return true; // Success
}

bool SceneSystem::Update(ecs::World& world)
{
    for (const ecs::Entity& entity : world.View(SceneComponentHandle::ID())())
    {
        SceneComponent* scene_component
            = world.GetComponent<SceneComponent>(entity, SceneComponentHandle::ID());
        assert(scene_component != nullptr && "Scene component is null!");

        switch (scene_component->GetCurrentState())
        {
        case SceneState::NeedsLoad:
            if (!scene_component->GetScene().LoadAssets(world))
                assert(false && "Failed to load scene assets!");
            scene_component->SetCurrentState(SceneState::Loading);
            break;

        case SceneState::Loading:
            if (scene_component->GetScene().IsAssetsLoaded(world))
            {
                scene_component->GetScene().Create(world);
                scene_component->SetCurrentState(SceneState::Playing);
            }
            break;

        case SceneState::Playing:
        {
            // Get meta component
            mono_meta_extension::MetaComponent* meta_component
                = world.GetComponent<mono_meta_extension::MetaComponent>(
                    entity, mono_meta_extension::MetaComponentHandle::ID());
            assert(meta_component != nullptr && "Entity who has scene component must have meta component!");

            // Get window entities for the scene
            std::vector<ecs::Entity> window_entities 
                = GetEntitiesForScene(
                    world, scene_component->GetRequiredWindowComponentIDs(), scene_component->GetSceneID());
            assert(!window_entities.empty() && "No window entity found for the scene!");
            assert(window_entities.size() <= 1 && "Multiple window entities found for the scene!");

            // Set window entities to scene component
            scene_component->SetWindowEntity(window_entities.front());

            // Get main camera entities for the scene
            std::vector<ecs::Entity> camera_entities
                = GetEntitiesForScene(
                    world, scene_component->GetRequiredCameraComponentIDs(), scene_component->GetSceneID());
            assert(!camera_entities.empty() && "No camera entity found for the scene!");
            assert(camera_entities.size() <= 1 && "Multiple camera entities found for the scene!");

            // Set main camera entity to scene component
            scene_component->SetMainCameraEntity(camera_entities.front());

            // Get renderable entities for the scene
            std::vector<ecs::Entity> renderable_entities
                = GetEntitiesForScene(
                    world, scene_component->GetRequiredRenderableComponentIDs(), scene_component->GetSceneID());

            // Set renderable entities to scene component
            scene_component->SetRenderableEntities(std::move(renderable_entities));

            // Get UI entities for the scene
            std::vector<ecs::Entity> ui_entities
                = GetEntitiesForScene(
                    world, scene_component->GetRequiredUIComponentIDs(), scene_component->GetSceneID());
            
            // Set UI entities to scene component
            scene_component->SetUIEntities(std::move(ui_entities));

            // Get light entities for the scene
            std::vector<std::pair<ecs::ComponentID, ecs::Entity>> light_entities
                = GetEntitiesForSceneWhoHasAnyComponent(
                    world, scene_component->GetLightComponentIDs(), scene_component->GetSceneID());

            // Set light entities to scene component
            scene_component->SetLightEntities(std::move(light_entities));
        }
            break;

        case SceneState::NeedsRelease:
            scene_component->GetScene().Destroy(world);
            scene_component->GetScene().ReleaseAssets(world);
            scene_component->SetCurrentState(SceneState::Releasing);

        case SceneState::Releasing:
            if (scene_component->GetScene().IsAssetsReleased(world))
                scene_component->SetCurrentState(SceneState::Released);
			break;

        default:
            break;
        }
    }

    return true; // Success
}

bool SceneSystem::PostUpdate(ecs::World& world)
{
    return true; // Success
}

ecs::SystemID SceneSystem::GetID() const
{
    return SceneSystemHandle::ID();
}

} // namespace mono_scene_extension