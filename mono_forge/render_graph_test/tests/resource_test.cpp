#include "render_graph_test/pch.h"

#include "directx12_util/include/wrapper.h"

#include "render_graph/include/resource_manager.h"
#include "render_graph/include/heap_manager.h"

namespace resource_test
{

struct TestHandleTag {};

} // namespace resource_test

TEST(Resource, ResourceManager)
{
    // Create a resource container
    std::unique_ptr<render_graph::ResourceContainer> resource_container 
        = std::make_unique<render_graph::ResourceContainer>();

    // Create singleton resource manager
    std::unique_ptr<render_graph::ResourceManager> resource_manager 
        = std::make_unique<render_graph::ResourceManager>(*resource_container);

    // Create resource adder
    std::unique_ptr<render_graph::ResourceAdder> resource_adder 
        = std::make_unique<render_graph::ResourceAdder>(*resource_container);

    // Create resource eraser
    std::unique_ptr<render_graph::ResourceEraser> resource_eraser
        = std::make_unique<render_graph::ResourceEraser>(*resource_container);

    // Create dx12 factory
    std::unique_ptr<dx12_util::DXFactory> dx_factory = std::make_unique<dx12_util::DXFactory>();
    ASSERT_TRUE(dx_factory->Setup());

    // Create dx12 device
    std::unique_ptr<dx12_util::Device> dx_device = std::make_unique<dx12_util::Device>();
    ASSERT_TRUE(dx_device->Setup(dx_factory->Get()));

    // Create buffer resource
    std::unique_ptr<dx12_util::Buffer> resource 
        = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
            256, D3D12_HEAP_TYPE_UPLOAD, L"Test Buffer Resource",
            dx_device->Get(), nullptr);

    // Add the resource and get its handle
    render_graph::ResourceHandle handle = resource_adder->AddResource(std::move(resource));

    resource_manager->WithLock([&](render_graph::ResourceManager& mgr) 
    {
        // Create an access token and permit access to the resource
        render_graph::ResourceAccessToken token;
        token.PermitAccess(&handle);

        dx12_util::Resource& accessed_resource = mgr.GetWriteResource(&handle, token);
        dx12_util::Buffer* buffer = dynamic_cast<dx12_util::Buffer*>(&accessed_resource);
        ASSERT_NE(buffer, nullptr);
    });

    // Erase the resource
    resource_eraser->EraseResource(&handle);
}

TEST(Resource, HeapManager)
{
    // Create dx12 factory
    std::unique_ptr<dx12_util::DXFactory> dx_factory = std::make_unique<dx12_util::DXFactory>();
    ASSERT_TRUE(dx_factory->Setup());

    // Create dx12 device
    std::unique_ptr<dx12_util::Device> dx_device = std::make_unique<dx12_util::Device>();
    ASSERT_TRUE(dx_device->Setup(dx_factory->Get()));

    // Create descriptor heaps
    constexpr UINT DESCRIPTOR_COUNT = 10;

    std::unique_ptr<dx12_util::DescriptorHeap> srv_heap 
        = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
            D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, dx_device->Get());

    std::unique_ptr<dx12_util::DescriptorHeap> rtv_heap 
        = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
            D3D12_DESCRIPTOR_HEAP_TYPE_RTV, DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, dx_device->Get());

    std::unique_ptr<dx12_util::DescriptorHeap> dsv_heap 
        = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
            D3D12_DESCRIPTOR_HEAP_TYPE_DSV, DESCRIPTOR_COUNT, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, dx_device->Get());

    // Create descriptor heap allocators
    std::unique_ptr<dx12_util::DescriptorHeapAllocator> srv_heap_allocator
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *srv_heap, dx_device->Get());

    std::unique_ptr<dx12_util::DescriptorHeapAllocator> rtv_heap_allocator
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *rtv_heap, dx_device->Get());

    std::unique_ptr<dx12_util::DescriptorHeapAllocator> dsv_heap_allocator
        = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
            *dsv_heap, dx_device->Get());

    // Create singleton heap manager
    std::unique_ptr<render_graph::HeapManager> heap_manager
        = std::make_unique<render_graph::HeapManager>(
            *srv_heap, *srv_heap_allocator,
            *rtv_heap, *rtv_heap_allocator,
            *dsv_heap, *dsv_heap_allocator);

    // Access the heaps and allocators in a thread-safe manner
    heap_manager->WithUniqueLock([&](render_graph::HeapManager& mgr)
    {
        dx12_util::DescriptorHeap& srv = mgr.GetSrvHeap();
        dx12_util::DescriptorHeapAllocator& srv_allocator = mgr.GetSrvHeapAllocator();
        ASSERT_EQ(srv.GetType(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        ASSERT_EQ(srv.GetDescriptorCount(), DESCRIPTOR_COUNT);

        dx12_util::DescriptorHeap& rtv = mgr.GetRtvHeap();
        dx12_util::DescriptorHeapAllocator& rtv_allocator = mgr.GetRtvHeapAllocator();
        ASSERT_EQ(rtv.GetType(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        ASSERT_EQ(rtv.GetDescriptorCount(), DESCRIPTOR_COUNT);

        dx12_util::DescriptorHeap& dsv = mgr.GetDsvHeap();
        dx12_util::DescriptorHeapAllocator& dsv_allocator = mgr.GetDsvHeapAllocator();
        ASSERT_EQ(dsv.GetType(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        ASSERT_EQ(dsv.GetDescriptorCount(), DESCRIPTOR_COUNT);
    });
}