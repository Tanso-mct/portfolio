#include "memory_allocator/src/pch.h"
#include "memory_allocator/include/fixed_block_allocator.h"

namespace memory_allocator
{

FixedBlockAllocator::FixedBlockAllocator(Pool& pool, size_t block_size) :
    Allocator(pool, block_size)
{
    // Initialize the free list
    for (size_t i = 0; i < block_count_; ++i)
    {
        FreeBlock* block = reinterpret_cast<FreeBlock*>(pool_.Get() + i * block_size_);
        block->next = free_list_;
        free_list_ = block;
    }
}

memory_allocator::FixedBlockAllocator::~FixedBlockAllocator()
{
    free_list_ = nullptr; // Clear the free list
}

std::byte* FixedBlockAllocator::Allocate(size_t size)
{
    assert(size <= block_size_); // Size must be less than or equal to block size
    assert(free_list_ != nullptr); // Ensure there is a free block available

    // Pop a block from the free list
    FreeBlock* block = free_list_;
    free_list_ = free_list_->next;

    // Return the allocated block
    return reinterpret_cast<std::byte*>(block);
}

void FixedBlockAllocator::Deallocate(std::byte** ptr)
{
    assert(ptr != nullptr); // Pointer must not be null
    assert(*ptr != nullptr); // Pointer to memory must not be null

    // Push the block back to the free list
    FreeBlock* block = reinterpret_cast<FreeBlock*>(*ptr);
    block->next = free_list_;
    free_list_ = block;

    // Reset the memory to nullptr
    *ptr = nullptr;
}

} // namespace memory_allocator