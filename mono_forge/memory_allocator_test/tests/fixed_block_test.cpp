#include "memory_allocator_test/pch.h"

#include "memory_allocator/include/fixed_block_allocator.h"
#pragma comment(lib, "memory_allocator")

TEST(FixedBlockAllocator, AllocateDeallocate)
{
    const size_t pool_size = 1024;
    const size_t block_size = 64;
    memory_allocator::Pool pool(pool_size);
    memory_allocator::FixedBlockAllocator allocator(pool, block_size);

    // Allocate a block
    std::byte* block = allocator.Allocate(block_size);
    ASSERT_NE(block, nullptr);

    // Deallocate the block
    allocator.Deallocate(&block);
    ASSERT_EQ(block, nullptr);
}