#include "memory_allocator/src/pch.h"
#include "memory_allocator/include/allocator.h"

namespace memory_allocator
{

Allocator::Allocator(Pool& pool, size_t block_size) :
    pool_(pool),
    block_size_(block_size),
    block_count_(pool_.GetSize() / block_size)
{
    assert(block_size_ > 0); // Block size must be greater than zero
    assert(block_count_ > 0); // Pool size must be greater than block size
}

} // namespace memory_allocator