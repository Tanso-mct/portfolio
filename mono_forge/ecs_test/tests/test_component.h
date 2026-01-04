#pragma once

#include <iostream>

#include "ecs/include/component.h"
#include "memory_allocator/include/fixed_block_allocator.h"

namespace ecs_test
{

// The handle for the test component
class TestComponentHandle :
    public ecs::ComponentHandle<TestComponentHandle> {};

// A test component implementation
class TestComponent :
    public ecs::Component
{
public:
    TestComponent()
    {
        std::cout << "TestComponent constructed\n";
    }

    virtual ~TestComponent() override
    {
        std::cout << "TestComponent destructed\n";
    }

    class SetupParam : public ecs::Component::SetupParam
    {
    public:
        int value = 0;
    };

    bool Setup(ecs::Component::SetupParam& param) override
    {
        // Try to cast the param to SetupParam
        SetupParam* test_param = dynamic_cast<SetupParam*>(&param);

        // If the cast fails, return false
        if (!test_param)
            return false;

        // Store the value from the setup param
        data_ = test_param->value;

        return true; // Setup successful
    }

    ecs::ComponentID GetID() const override
    {
        return TestComponentHandle::ID();
    }

    int GetData() const { return data_; }

private:
    int data_ = 0;
};

// The factory for creating allocators for the test component
class TestComponentAllocatorFactory :
    public ecs::IComponentAllocatorFactory
{
public:
    std::unique_ptr<memory_allocator::Allocator> Create(memory_allocator::Pool& pool, size_t block_size) const override
    {
        return std::make_unique<memory_allocator::FixedBlockAllocator>(pool, block_size);
    }

    void Destroy(std::unique_ptr<memory_allocator::Allocator>& allocator) const override
    {
        allocator.reset();
    }

    size_t GetProductSize() const override
    {
        return sizeof(memory_allocator::FixedBlockAllocator);
    }
};

} // namespace ecs_test