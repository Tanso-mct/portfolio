#pragma once

#include <cstddef>
#include <memory>

#include "memory_allocator/include/dll_config.h"
#include "memory_allocator/include/allocator.h"

namespace memory_allocator
{

class MEMORY_ALLOCATOR_DLL FixedBlockAllocator :
    public Allocator
{
public:
    FixedBlockAllocator(Pool& pool, size_t block_size);
    virtual ~FixedBlockAllocator();

    // Allocate a block of memory of the given size
    std::byte* Allocate(size_t size) override;

    // Deallocate a previously allocated block of memory
    void Deallocate(std::byte** ptr) override;

private:
    struct FreeBlock { FreeBlock *next; }; // Structure for free list
    FreeBlock *free_list_ = nullptr; // Pointer to the head of the free list
};

} // namespace memory_allocator