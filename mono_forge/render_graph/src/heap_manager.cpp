#include "render_graph/src/pch.h"
#include "render_graph/include/heap_manager.h"

namespace render_graph
{

dx12_util::DescriptorHeap& HeapManager::GetSrvHeap()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return srv_heap_;
}

dx12_util::DescriptorHeapAllocator& HeapManager::GetSrvHeapAllocator()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return srv_heap_allocator_;
}

dx12_util::DescriptorHeap& HeapManager::GetRtvHeap()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return rtv_heap_;
}

dx12_util::DescriptorHeapAllocator& HeapManager::GetRtvHeapAllocator()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return rtv_heap_allocator_;
}

dx12_util::DescriptorHeap& HeapManager::GetDsvHeap()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return dsv_heap_;
}

dx12_util::DescriptorHeapAllocator& HeapManager::GetDsvHeapAllocator()
{
    assert(IsUniqueLock()); // Ensure called within WithUniqueLock lambda function
    return dsv_heap_allocator_;
}

} // namespace render_graph