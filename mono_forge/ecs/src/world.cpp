#include "ecs/src/pch.h"
#include "ecs/include/world.h"

namespace ecs
{

World::World(std::unique_ptr<ComponentDescriptorRegistry> component_descriptor_registry) :
    component_descriptor_registry_(std::move(component_descriptor_registry))
{
    assert(component_descriptor_registry_ != nullptr); // Ensure the component registry is not null
    
    // Get all registered component ids
    std::vector<size_t> component_ids;
    component_descriptor_registry_->WithSharedLock([&](const ComponentDescriptorRegistry& registry)
    {
        component_ids = registry.GetRegisteredIDs();
    });

    // Reserve memory for component pools and allocators
    component_pools_.reserve(component_ids.size());
    component_allocators_.reserve(component_ids.size());

    // Create memory pools and allocators for each component type
    for (size_t component_id : component_ids)
    {
        component_descriptor_registry_->WithUniqueLock([&](const ComponentDescriptorRegistry& registry)
        {
            assert(registry.Contains(component_id)); // Ensure the component ID is registered

            // Get the component descriptor
            ComponentDescriptor& desc = registry.Get(component_id);

            // Create the memory pool for the component type
            std::unique_ptr<memory_allocator::Pool> pool 
                = std::make_unique<memory_allocator::Pool>(desc.GetMaxCount() * desc.GetSize());

            // Create the allocator for the component type
            std::unique_ptr<memory_allocator::Allocator> allocator 
                = desc.GetAllocatorFactory().Create(*pool, desc.GetSize());

            // Store the pool and allocator
            component_pools_[component_id] = std::move(pool);
            component_allocators_[component_id] = std::move(allocator);
        });
    }
}

World::~World()
{
    // Clean up all entities and their components
    for (const Entity& entity : entities_)
        if (CheckEntityExist(entity))
            DestroyEntity(entity);

    // Destroy all component allocators by using their factories
    for (const auto& [component_id, allocator] : component_allocators_)
    {
        component_descriptor_registry_->WithUniqueLock([&](const ComponentDescriptorRegistry& registry)
        {
            assert(registry.Contains(component_id)); // Ensure the component ID is registered

            // Get the component descriptor
            ComponentDescriptor& desc = registry.Get(component_id);

            // Use the factory to destroy the allocator
            desc.GetAllocatorFactory().Destroy(component_allocators_[component_id]);
        });
    }
}

Entity World::CreateEntity()
{
    std::unique_lock<std::shared_mutex> lock = LockUnique(); // Lock for exclusive access

    if (!free_entities_.empty()) // Reuse a free entity
    {
        // Get the last free entity
        Entity entity = free_entities_.back();
        free_entities_.pop_back();

        // Mark the entity as existing
        entity_exist_flags_[entity.GetIndex()] = true;

        // Update the generation of the entity
        entities_[entity.GetIndex()] = Entity(entity.GetIndex(), entity.GetGeneration() + 1);

        return entities_[entity.GetIndex()]; // Return the reused entity
    }
    else // Create a new entity
    {
        // Get the new entity index
        size_t index = entities_.size();

        // Create a new entity with default generation
        entities_.emplace_back(index, utility_header::DEFAULT_GENERATION);

        // Mark the new entity as existing
        entity_exist_flags_.emplace_back(true);
        
        return entities_.back(); // Return the newly created entity
    }
}

bool World::DestroyEntity(const Entity& entity)
{
    std::unique_lock<std::shared_mutex> lock = LockUnique(); // Lock for exclusive access

    assert(entity.GetIndex() < entities_.size()); // Ensure the entity index is within bounds
    assert(entity_exist_flags_[entity.GetIndex()]); // Ensure the entity exists
    assert(entities_[entity.GetIndex()] == entity); // Ensure the entity matches the stored

    // Remove all components associated with the entity
    if (entity_to_component_ids_.find(entity) != entity_to_component_ids_.end())
    {
        // Copy the component IDs to avoid modifying the set while iterating
        std::vector<ComponentID> component_ids(
            entity_to_component_ids_[entity].begin(), entity_to_component_ids_[entity].end());

        for (ComponentID component_id : component_ids)
        {
            lock.unlock(); // Unlock before calling RemoveComponentWithoutLock to avoid deadlock
            RemoveComponent(entity, component_id); // Remove the component
            lock.lock(); // Re-lock after removing the component
        }
    }

    // Mark the entity as not existing
    entity_exist_flags_[entity.GetIndex()] = false;

    // Add the entity to the free list for reuse
    free_entities_.push_back(entity);

    return true; // Entity destroyed successfully
}

bool World::CheckEntityExist(const Entity& entity) const
{
    std::shared_lock<std::shared_mutex> lock = LockShared(); // Lock for shared access

    if (entity.GetIndex() >= entities_.size()) // If the index is out of bounds, the entity does not exist
        return false;

    // The entity exists if the exist flag is true and the entity matches the stored one
    return entity_exist_flags_[entity.GetIndex()] && entities_[entity.GetIndex()] == entity;
}

bool World::RemoveComponent(const Entity& entity, ComponentID component_id)
{
    assert(CheckEntityExist(entity)); // Ensure the entity exists
    assert(HasComponent(entity, component_id)); // Ensure the entity has the component
    
    component_descriptor_registry_->WithSharedLock([&](const ComponentDescriptorRegistry& registry)
    {
        assert(registry.Contains(component_id)); // Ensure the component ID is registered
    });

    std::unique_lock<std::shared_mutex> lock = LockUnique(); // Lock for exclusive access

    assert(component_pools_.find(component_id) != component_pools_.end()); // Ensure the component pool exists
    assert(component_allocators_.find(component_id) != component_allocators_.end()); // Ensure the component allocator exists
    assert(entity_component_to_data_.find({entity, component_id}) != entity_component_to_data_.end()); // Ensure the component data exists

    // Get the component data
    Component* component = entity_component_to_data_[{entity, component_id}];

    // Destroy the component instance using the destructor
    component->~Component();

    // Free the memory allocated for the component using the allocator
    component_allocators_[component_id]->Deallocate(reinterpret_cast<std::byte**>(&component));

    // Remove the component data from the maps
    entity_to_component_ids_[entity].erase(component_id);
    component_id_to_entities_[component_id].erase(entity);
    entity_component_to_data_.erase({entity, component_id});

    return true; // Component removed successfully
}

bool World::HasComponent(const Entity& entity, ComponentID component_id) const
{
    assert(CheckEntityExist(entity)); // Ensure the entity exists

    component_descriptor_registry_->WithSharedLock([&](const ComponentDescriptorRegistry& registry)
    {
        assert(registry.Contains(component_id)); // Ensure the component ID is registered
    });

    std::shared_lock<std::shared_mutex> lock = LockShared(); // Lock for shared access

    // Check if the entity has the component by looking it up in the map
    auto it = entity_to_component_ids_.find(entity);
    if (it != entity_to_component_ids_.end())
        return it->second.find(component_id) != it->second.end();
    else
        return false; // The entity has no components
}

utility_header::ConstSharedLockedValue<std::set<ComponentID>> World::GetComponentIDs(const Entity& entity) const
{
    assert(CheckEntityExist(entity)); // Ensure the entity exists

    std::shared_lock<std::shared_mutex> lock = LockShared(); // Lock for shared access

    // Find the component IDs associated with the entity
    auto it = entity_to_component_ids_.find(entity);

    if (it != entity_to_component_ids_.end()) // If found, return the set of component IDs
        return utility_header::ConstSharedLockedValue<std::set<ComponentID>>(it->second, std::move(lock));
    else // Not found, return an empty set
        return utility_header::ConstSharedLockedValue<std::set<ComponentID>>(empty_component_ids_, std::move(lock));
}

utility_header::ConstSharedLockedValue<std::set<Entity>> World::View(ComponentID component_id) const
{
    component_descriptor_registry_->WithSharedLock([&](const ComponentDescriptorRegistry& registry)
    {
        assert(registry.Contains(component_id)); // Ensure the component ID is registered
    });

    std::shared_lock<std::shared_mutex> lock = LockShared(); // Lock for shared access

    // Find the entities that have the specified component
    auto it = component_id_to_entities_.find(component_id);

    if (it != component_id_to_entities_.end()) // If found, return the set of entities
        return utility_header::ConstSharedLockedValue<std::set<Entity>>(it->second, std::move(lock));
    else // Not found, return an empty set
        return utility_header::ConstSharedLockedValue<std::set<Entity>>(empty_entities_, std::move(lock));
}

} // namespace ecs