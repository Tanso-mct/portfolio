#include "directx12_util_test/pch.h"

#include "directx12_util/include/wrapper.h"

namespace init_test
{

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_SIZE:
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // ALTメニューの無効化
            return 0;
        break;

    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

} // namespace init_test

TEST(Init, Device)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);
}

TEST(Init, Command)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    // Create the CommandQueue
    std::unique_ptr<dx12_util::CommandQueue> command_queue 
        = std::make_unique<dx12_util::CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
    result = command_queue->Setup(device->Get());
    ASSERT_NE(result, false);

    {
        // Create the CommandAllocator
        std::unique_ptr<dx12_util::CommandAllocator> command_allocator 
            = dx12_util::CommandAllocator::CreateInstance<dx12_util::CommandAllocator>(
                D3D12_COMMAND_LIST_TYPE_DIRECT, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(command_allocator, nullptr);
        ASSERT_NE(command_allocator->Get(), nullptr);

        // Create the CommandList
        std::unique_ptr<dx12_util::CommandList> command_list
            = dx12_util::CommandList::CreateInstance<dx12_util::CommandList>(
                *command_allocator, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(command_list, nullptr);
        ASSERT_NE(command_list->Get(), nullptr);
    }
}

TEST(Init, DescriptorHeap)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    {
        // Create the DescriptorHeap
        const D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        const UINT descriptor_count = 10;
        const D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        std::unique_ptr<dx12_util::DescriptorHeap> descriptor_heap 
            = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
                type, descriptor_count, flags, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(descriptor_heap, nullptr);

        // Create the DescriptorHeapAllocator
        std::unique_ptr<dx12_util::DescriptorHeapAllocator> descriptor_heap_allocator
            = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
                *descriptor_heap, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(descriptor_heap_allocator, nullptr);
    }
}

TEST(Init, SwapChain)
{
    bool result = false;

    // Register the window class
    WNDCLASSEXW wc = 
    { 
        sizeof(wc), CS_CLASSDC, init_test::WndProc, 0L, 0L, 
        GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"InitSwapChain", nullptr 
    };
    ::RegisterClassExW(&wc);

    // Create the application's window
    HWND hwnd = ::CreateWindowW
    (
        wc.lpszClassName, L"InitSwapChain", WS_OVERLAPPEDWINDOW, 
        100, 100, 800, 600, 
        nullptr, nullptr, wc.hInstance, nullptr
    );

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Get client area size
    RECT rect;
    ::GetClientRect(hwnd, &rect);
    const UINT client_width = rect.right - rect.left;
    const UINT client_height = rect.bottom - rect.top;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    // Create the CommandQueue
    std::unique_ptr<dx12_util::CommandQueue> command_queue 
        = std::make_unique<dx12_util::CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT);
    result = command_queue->Setup(device->Get());
    ASSERT_NE(result, false);

    {
        // Create the SwapChain
        const UINT frame_count = 2;
        const UINT sync_interval = 1;
        const DXGI_SWAP_EFFECT swap_effect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        std::unique_ptr<dx12_util::SwapChain> swap_chain 
            = dx12_util::SwapChain::CreateInstance<dx12_util::SwapChain>(
                frame_count, sync_interval, swap_effect, format, L"InitSwapChain",
                dx12_util::Device::GetInstance().Get(), dx12_util::DXFactory::GetInstance().Get(),
                dx12_util::CommandQueue::GetInstance().Get(), hwnd, client_width, client_height);
        ASSERT_NE(swap_chain, nullptr);
    }

    // Destroy the window
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    ::DestroyWindow(hwnd);
}

TEST(Init, Buffer)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    {
        // Create the Buffer
        const uint32_t size = 256;
        const D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_UPLOAD;
        std::unique_ptr<dx12_util::Buffer> buffer 
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                size, heap_type, L"InitBuffer", dx12_util::Device::GetInstance().Get(), nullptr);
        ASSERT_NE(buffer, nullptr);
        ASSERT_NE(buffer->Get(), nullptr);
    }
}

TEST(Init, Texture2D)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    {
        // Create the DescriptorHeap
        const D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        const UINT descriptor_count = 10;
        const D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        std::unique_ptr<dx12_util::DescriptorHeap> descriptor_heap 
            = dx12_util::DescriptorHeap::CreateInstance<dx12_util::DescriptorHeap>(
                type, descriptor_count, flags, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(descriptor_heap, nullptr);

        // Create the DescriptorHeapAllocator
        std::unique_ptr<dx12_util::DescriptorHeapAllocator> descriptor_heap_allocator
            = dx12_util::DescriptorHeapAllocator::CreateInstance<dx12_util::DescriptorHeapAllocator>(
                *descriptor_heap, dx12_util::Device::GetInstance().Get());
        ASSERT_NE(descriptor_heap_allocator, nullptr);

        // Prepare parameters for Texture2D
        const UINT width = 256;
        const UINT height = 256;
        const DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
        const D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT;
        const D3D12_RESOURCE_FLAGS resource_flags = D3D12_RESOURCE_FLAG_NONE;

        // Create the Texture2D
        std::unique_ptr<dx12_util::Texture2D> texture 
            = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                width, height, format, heap_type, resource_flags, D3D12_RESOURCE_STATE_COMMON, L"InitTexture2D",
                dx12_util::Device::GetInstance().Get(), nullptr,
                nullptr, nullptr, nullptr, nullptr,
                descriptor_heap_allocator.get(), nullptr, nullptr);

        // Verify the texture properties
        ASSERT_NE(texture, nullptr);
        ASSERT_NE(texture->Get(), nullptr);
        ASSERT_EQ(texture->GetWidth(), width);
        ASSERT_EQ(texture->GetHeight(), height);
        ASSERT_EQ(texture->GetFormat(), format);
        ASSERT_EQ(texture->GetHeapType(), heap_type);
        ASSERT_EQ(texture->GetResourceFlags(), resource_flags);
    }
}

TEST(Init, Fence)
{
    bool result = false;

    // Create the DXFactory
    std::unique_ptr<dx12_util::DXFactory> factory = std::make_unique<dx12_util::DXFactory>();
    result = factory->Setup();
    ASSERT_NE(result, false);

    // Create the Device
    std::unique_ptr<dx12_util::Device> device = std::make_unique<dx12_util::Device>();
    result = device->Setup(factory->Get());
    ASSERT_NE(result, false);

    {
        // Create the Fence
        std::unique_ptr<dx12_util::Fence> fence 
            = dx12_util::Fence::CreateInstance<dx12_util::Fence>(
                0, D3D12_FENCE_FLAG_NONE, 100000,
                dx12_util::Device::GetInstance().Get());
        ASSERT_NE(fence, nullptr);
        ASSERT_NE(fence->Get(), nullptr);
    }
}