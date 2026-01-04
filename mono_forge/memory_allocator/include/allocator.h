#pragma once

#include "memory_allocator/include/dll_config.h"
#include "memory_allocator/include/pool.h"

namespace memory_allocator
{

class MEMORY_ALLOCATOR_DLL Allocator
{
public:
    Allocator(Pool& pool, size_t block_size);
    virtual ~Allocator() = default;

    // Allocate a block of memory of the given size
    virtual std::byte* Allocate(size_t size) = 0;

    // Deallocate a previously allocated block of memory
    virtual void Deallocate(std::byte** ptr) = 0;

protected:
    Pool& pool_; // The memory pool used for allocation
    const size_t block_size_; // The size of each block in the allocator
    const size_t block_count_; // The number of blocks in the pool
};

} // namespace memory_allocator