#include "memory_allocator/src/pch.h"
#include "memory_allocator/include/pool.h"

namespace memory_allocator
{

Pool::Pool(size_t pool_size) :
    pool_(std::make_unique<std::byte[]>(pool_size)),
    pool_size_(pool_size)
{
    assert(pool_size > 0); // Pool size must be greater than zero
}

} // namespace memory_allocator