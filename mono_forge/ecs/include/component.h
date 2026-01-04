#pragma once

#include <cstddef>
#include <cassert>

#include "class_template/singleton.h"
#include "class_template/factory.h"

#include "utility_header/registry.h"
#include "utility_header/id.h"

#include "memory_allocator/include/pool.h"
#include "memory_allocator/include/allocator.h"

#include "ecs/include/dll_config.h"

namespace ecs
{

// The type used to identify component types
using ComponentID = utility_header::ID;

// The singleton ID generator for component types
class ECS_DLL ComponentIDGenerator :
    public class_template::Singleton<ComponentIDGenerator>,
    public utility_header::IDGenerator
{
public:
    ComponentIDGenerator() = default;
    virtual ~ComponentIDGenerator() override = default;
};

// The interface for all components
class Component :
    public class_template::NonCopyable
{
public:
    Component() = default;
    virtual ~Component() = default;

    // The parameters used to setup the component
    // You can inherit this class to create your own setup parameters
    class SetupParam
    {
    public:
        virtual ~SetupParam() = default;
    };

    // Setup the component with the given parameters
    // It always called after the component instance is created
    // Return true if the setup is successful, false otherwise
    virtual bool Setup(SetupParam& param) = 0;

    // Apply changes to the component with the given parameters
    // It can be called to update the component after setup
    virtual bool Apply(const SetupParam& param)
    {
        assert(false && "Calling Apply is not implemented for this component");
        return false; // Not implemented
    }

    // Get the component ID
    virtual ComponentID GetID() const = 0;
};

// The template class for component handles of specific types
template <typename ComponentType>
class ComponentHandle
{
public:
    ComponentHandle() = default;
    virtual ~ComponentHandle() = default;

    // Get the unique ID of the component type
    static ComponentID ID()
    {
        static const ComponentID id = ComponentIDGenerator::GetInstance().Generate();
        return id;
    }
};

// The factory interface for creating component allocators
// It returns a unique pointer to the created allocator
using IComponentAllocatorFactory 
    = class_template::IFactory<std::unique_ptr<memory_allocator::Allocator>, memory_allocator::Pool&, size_t>;

// The descriptor for a component type
// It contains the component ID, maximum count, and the factory for creating and destroying component instances
// It is used to register component types in the ECS world
class ECS_DLL ComponentDescriptor
{
public:
    ComponentDescriptor(
        size_t size_, size_t max_count, std::unique_ptr<IComponentAllocatorFactory> allocator_factory);
    ~ComponentDescriptor() = default;

    // Get the size of the component type
    size_t GetSize() const { return size_; }

    // Get the maximum number of component instances
    size_t GetMaxCount() const { return max_count_; }

    // The factory for creating component allocators
    const IComponentAllocatorFactory& GetAllocatorFactory() const { return *allocator_factory_; }

private:
    const size_t size_; // The size of the component type
    const size_t max_count_; // The maximum number of component instances
    const std::unique_ptr<IComponentAllocatorFactory> allocator_factory_; // The factory for creating component allocators
};

// The registry for component descriptors
using ComponentDescriptorRegistry = utility_header::Registry<ComponentDescriptor>;

// Helper function to register a component descriptor
template <typename ComponentType, typename AllocatorFactory, typename ComponentHandleType>
void RegisterComponentDescriptor(
    ecs::ComponentDescriptorRegistry& component_descriptor_registry, size_t max_count)
{
    // Register component descriptor
    {
        std::unique_ptr<ecs::ComponentDescriptor> component_desc
            = std::make_unique<ecs::ComponentDescriptor>(
                sizeof(ComponentType),
                max_count,
                std::make_unique<AllocatorFactory>());

        component_descriptor_registry.WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
        {
            // Register the descriptor with component ID
            registry.Register(
                ComponentHandleType::ID(), std::move(component_desc));
        });
    }
}

// The registrar for component descriptors
template <typename ComponentType, typename AllocatorFactory, typename ComponentHandleType>
class ComponentDescriptorRegistrar
{
public:
    ComponentDescriptorRegistrar(
        ecs::ComponentDescriptorRegistry& registry, size_t max_count)
    {
        // Register the component descriptor
        RegisterComponentDescriptor<ComponentType, AllocatorFactory, ComponentHandleType>(registry, max_count);
    }

    ~ComponentDescriptorRegistrar() = default;
};

} // namespace ecs