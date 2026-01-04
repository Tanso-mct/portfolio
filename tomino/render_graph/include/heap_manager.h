#pragma once

#include "class_template/singleton.h"
#include "class_template/thread_safer.h"
#include "directx12_util/include/wrapper.h"

#include "render_graph/include/dll_config.h"

namespace render_graph
{

// The heap manager class that manages descriptor heaps and their allocators
// It provides thread-safe access to the heaps and allocators
class RENDER_GRAPH_DLL HeapManager :
    public class_template::Singleton<HeapManager>,
    public class_template::WithLocker<HeapManager>
{
public:
    HeapManager(
        dx12_util::DescriptorHeap& srv_heap, dx12_util::DescriptorHeapAllocator& srv_heap_allocator,
        dx12_util::DescriptorHeap& rtv_heap, dx12_util::DescriptorHeapAllocator& rtv_heap_allocator,
        dx12_util::DescriptorHeap& dsv_heap, dx12_util::DescriptorHeapAllocator& dsv_heap_allocator) :
        srv_heap_(srv_heap), 
        srv_heap_allocator_(srv_heap_allocator),
        rtv_heap_(rtv_heap),
        rtv_heap_allocator_(rtv_heap_allocator),
        dsv_heap_(dsv_heap),
        dsv_heap_allocator_(dsv_heap_allocator)
    {
    }

    ~HeapManager() override = default;

    // Get the SRV descriptor heap
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeap& GetSrvHeap();

    // Get the SRV descriptor heap allocator
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeapAllocator& GetSrvHeapAllocator();

    // Get the RTV descriptor heap
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeap& GetRtvHeap();

    // Get the RTV descriptor heap allocator
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeapAllocator& GetRtvHeapAllocator();

    // Get the DSV descriptor heap
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeap& GetDsvHeap();

    // Get the DSV descriptor heap allocator
    // It must be called in WithUniqueLock lambda function
    dx12_util::DescriptorHeapAllocator& GetDsvHeapAllocator();

private:
    // Descriptor heap for shader resource views (SRV)
    dx12_util::DescriptorHeap& srv_heap_;

    // Descriptor heap allocator for shader resource views (SRV)
    dx12_util::DescriptorHeapAllocator& srv_heap_allocator_;

    // Descriptor heap for render target views (RTV)
    dx12_util::DescriptorHeap& rtv_heap_;

    // Descriptor heap allocator for render target views (RTV)
    dx12_util::DescriptorHeapAllocator& rtv_heap_allocator_;

    // Descriptor heap for depth stencil views (DSV)
    dx12_util::DescriptorHeap& dsv_heap_;

    // Descriptor heap allocator for depth stencil views (DSV)
    dx12_util::DescriptorHeapAllocator& dsv_heap_allocator_;

};

} // namespace render_graph