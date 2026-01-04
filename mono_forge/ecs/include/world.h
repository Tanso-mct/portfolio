#pragma once

#include <cassert>
#include <unordered_map>
#include <set>
#include <vector>
#include <memory>

#include "class_template/instance.h"
#include "class_template/thread_safer.h"

#include "utility_header/container.h"
#include "utility_header/registry.h"
#include "utility_header/locked_value.h"

#include "memory_allocator/include/pool.h"
#include "memory_allocator/include/allocator.h"

#include "ecs/include/dll_config.h"
#include "ecs/include/entity.h"
#include "ecs/include/component.h"

namespace ecs
{

// The main world class that manages entities and components
// It provides methods to create/destroy entities, add/remove components, and query entities
// It is designed to be thread-safe
class ECS_DLL World :
    public class_template::ThreadSafer
{
public:
    World(std::unique_ptr<ComponentDescriptorRegistry> component_descriptor_registry);
    ~World();

    // Create a new entity
    Entity CreateEntity();

    // Destroy an existing entity
    bool DestroyEntity(const Entity &entity);

    // Check if an entity exists
    bool CheckEntityExist(const Entity &entity) const;

    // Add a component to an entity
    // If setup_param is not null, it will be used to setup the component after creation
    // Template T is the component concrete type
    // Template ConstructArgs are the types of the arguments passed to the component constructor
    template <typename T, typename... ConstructArgs>
    bool AddComponent(
        const Entity &entity, ComponentID component_id, 
        std::unique_ptr<Component::SetupParam> setup_param, ConstructArgs... construct_args)
    {
        assert(CheckEntityExist(entity)); // Ensure the entity exists
        assert(!HasComponent(entity, component_id)); // Ensure the entity does not already have the component

        std::unique_lock<std::shared_mutex> lock = LockUnique(); // Lock for exclusive access

        assert(component_pools_.find(component_id) != component_pools_.end()); // Ensure the component pool exists
        assert(component_allocators_.find(component_id) != component_allocators_.end()); // Ensure the component allocator exists

        // Get the component size
        size_t component_size = 0;
        component_descriptor_registry_->WithUniqueLock([&](const ComponentDescriptorRegistry& registry)
        {
            assert(registry.Contains(component_id)); // Ensure the component ID is registered
            component_size = registry.Get(component_id).GetSize();
        });

        // Allocate memory for the component using the allocator
        std::byte* component_data = component_allocators_[component_id]->Allocate(component_size);

        // Create the component instance using placement new
        Component* component = new (component_data) T(std::forward<ConstructArgs>(construct_args)...);

        // Setup the component if setup parameters are provided
        if (setup_param != nullptr && !component->Setup(*setup_param)) // If setup failed, clean up and return false
        {
            // Destroy the component instance using the destructor
            component->~Component();

            // Free the memory allocated for the component using the allocator
            component_allocators_[component_id]->Deallocate(reinterpret_cast<std::byte**>(&component));
            
            return false; // Setup failed
        }

        // Store the component data in the maps
        entity_to_component_ids_[entity].emplace(component_id);
        component_id_to_entities_[component_id].emplace(entity);
        entity_component_to_data_[{entity, component_id}] = component;

        return true; // Component added successfully
    }

    // Remove a component from an entity
    bool RemoveComponent(const Entity &entity, ComponentID component_id);

    // Check if an entity has a specific component
    bool HasComponent(const Entity &entity, ComponentID component_id) const;

    // Get the list of component IDs that an entity has
    utility_header::ConstSharedLockedValue<std::set<ComponentID>> GetComponentIDs(const Entity &entity) const;

    // Get a pointer to the component data of an entity
    // It cast the data to the specified type T
    // Returns nullptr if the entity does not have the component
    template <typename T>
    T* GetComponent(const Entity &entity, ComponentID component_id)
    {
        assert(CheckEntityExist(entity)); // Ensure the entity exists

        component_descriptor_registry_->WithSharedLock([&](const ComponentDescriptorRegistry& registry)
        {
            assert(registry.Contains(component_id)); // Ensure the component ID is registered
        });

        if (!HasComponent(entity, component_id)) // If the entity does not have the component, return nullptr
            return nullptr;

        std::shared_lock<std::shared_mutex> lock = LockShared(); // Lock for shared access

        // Find the component data associated with the entity and component ID
        auto it = entity_component_to_data_.find({entity, component_id});
        if (it != entity_component_to_data_.end())
            return dynamic_cast<T*>(it->second); // Return the component data casted to type T
        else
            return nullptr; // Component data not found, return nullptr
    }

    // Get a read-only view of all entities that have the specified component
    utility_header::ConstSharedLockedValue<std::set<Entity>> View(ComponentID component_id) const;

private:
    const std::unique_ptr<ComponentDescriptorRegistry> component_descriptor_registry_; // The registry of component descriptors

    std::vector<Entity> entities_; // The list of all entities
    std::vector<bool> entity_exist_flags_; // Flags indicating if an entity exists
    std::vector<Entity> free_entities_; // The list of free entities for reuse

    // The memory pools for each component type
    std::unordered_map<ComponentID, std::unique_ptr<memory_allocator::Pool>> component_pools_;

    // The allocators for each component type
    std::unordered_map<ComponentID, std::unique_ptr<memory_allocator::Allocator>> component_allocators_;

    // Map from entity to its component IDs
    std::unordered_map<Entity, std::set<ComponentID>> entity_to_component_ids_;
    std::set<ComponentID> empty_component_ids_; // An empty set of component IDs for entities with no components

    // Map from component ID to entities
    std::unordered_map<ComponentID, std::set<Entity>> component_id_to_entities_;
    std::set<Entity> empty_entities_; // An empty set of entities for components with no entities

    // Struct to represent a key for entity-component pair
    struct EntityComponentKey
    {
        Entity entity;
        ComponentID component_id;

        bool operator==(const EntityComponentKey &other) const
        {
            return entity == other.entity && component_id == other.component_id;
        }
    };

    // Hash function for EntityComponentKey to be used in unordered_map
    struct EntityComponentKeyHash
    {
        std::size_t operator()(const EntityComponentKey &key) const
        {
            return std::hash<Entity>()(key.entity) ^ (std::hash<ComponentID>()(key.component_id) << 1);
        }
    };

    // Map from entity-component pair to component data
    std::unordered_map<EntityComponentKey, Component*, EntityComponentKeyHash> entity_component_to_data_;
};

} // namespace ecs