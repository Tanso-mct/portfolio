#include "mono_entity_archive_extension/src/pch.h"
#include "mono_entity_archive_extension/include/allocator_factory.h"

#include "memory_allocator/include/fixed_block_allocator.h"

namespace mono_entity_archive_extension
{

std::unique_ptr<memory_allocator::Allocator> ComponentAllocatorFactory::Create(
    memory_allocator::Pool& pool, size_t block_size) const
{
    return std::make_unique<memory_allocator::FixedBlockAllocator>(pool, block_size);
}

void ComponentAllocatorFactory::Destroy(
    std::unique_ptr<memory_allocator::Allocator>& product) const
{
    product.reset();
}

size_t ComponentAllocatorFactory::GetProductSize() const
{
    return sizeof(memory_allocator::FixedBlockAllocator);
}

} // namespace mono_entity_archive_extension