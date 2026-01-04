#include "ecs/src/pch.h"
#include "ecs/include/component.h"

namespace ecs
{

ecs::ComponentDescriptor::ComponentDescriptor(
    size_t size, size_t max_count, std::unique_ptr<IComponentAllocatorFactory> allocator_factory) :
    size_(size),
    max_count_(max_count),
    allocator_factory_(std::move(allocator_factory))
{
    assert(size_ > 0); // Ensure the component size is greater than zero
    assert(max_count_ > 0); // Ensure the maximum count is greater than zero
    assert(allocator_factory_ != nullptr); // Ensure the allocator factory is not null
}

} // namespace ecs