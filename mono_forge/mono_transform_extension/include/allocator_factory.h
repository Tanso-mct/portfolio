#pragma once

#include "memory_allocator/include/allocator.h"
#include "ecs/include/component.h"

#include "mono_transform_extension/include/dll_config.h"

namespace mono_transform_extension
{
    
class MONO_TRANSFORM_EXT_DLL ComponentAllocatorFactory :
    public ecs::IComponentAllocatorFactory
{
public:
    ComponentAllocatorFactory() = default;
    virtual ~ComponentAllocatorFactory() override = default;

    virtual std::unique_ptr<memory_allocator::Allocator> Create(
        memory_allocator::Pool& pool, size_t block_size) const override;

    virtual void Destroy(
        std::unique_ptr<memory_allocator::Allocator>& product) const override;

    virtual size_t GetProductSize() const override;
};

} // namespace mono_transform_extension