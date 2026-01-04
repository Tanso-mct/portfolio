#pragma once

#include "ecs/include/system.h"

namespace ecs_test
{

// Example system A handle
class ExampleASystemHandle : 
    public ecs::SystemHandle<ExampleASystemHandle> {};

// Example system A
class ExampleASystem : 
    public ecs::System
{
public:
    ExampleASystem()
    {
    }

    virtual ~ExampleASystem() = default;

    virtual bool Update(ecs::World& world) override
    {
        // No operation, just return true
        return true;
    }

    virtual ecs::SystemID GetID() const override
    {
        return ExampleASystemHandle::ID();
    }
};

// Example system B handle
class ExampleBSystemHandle : 
    public ecs::SystemHandle<ExampleBSystemHandle> {};

// Example system B
class ExampleBSystem : 
    public ecs::System
{
public:
    ExampleBSystem()
    {
    }

    virtual ~ExampleBSystem() = default;

    virtual bool Update(ecs::World& world) override
    {
        // No operation, just return true
        return true;
    }

    virtual ecs::SystemID GetID() const override
    {
        return ExampleBSystemHandle::ID();
    }
};

} // namespace ecs_test