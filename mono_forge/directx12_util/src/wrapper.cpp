#include "directx12_util/src/pch.h"
#include "directx12_util/include/wrapper.h"

#include "directx12_util/include/d3dx12.h"
#include "utility_header/logger.h"

using Microsoft::WRL::ComPtr;

namespace dx12_util
{

bool DXFactory::Setup()
{
    // Enable debug layer in debug mode
    UINT dxgi_factory_flags = 0;
#ifdef _DEBUG
    {
        ComPtr<ID3D12Debug> debug_controller;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf()))))
        {
            debug_controller->EnableDebugLayer();

            // Enable additional debug layer features
            dxgi_factory_flags |= DXGI_CREATE_FACTORY_DEBUG;

            ComPtr<ID3D12Debug1> debug_controller_1;
            debug_controller->QueryInterface(IID_PPV_ARGS(debug_controller_1.GetAddressOf()));
            debug_controller_1->SetEnableGPUBasedValidation(true);
        }
    }
#endif

    // Create DX12 factory
    HRESULT hr = CreateDXGIFactory2(dxgi_factory_flags, IID_PPV_ARGS(factory_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create DXGI Factory."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Mark as created
    setup_flag_ = true;

    return true;
}

IDXGIFactory4* DXFactory::Get()
{
    assert(setup_flag_); // Ensure the instance is created
    return factory_.Get();
}

bool Device::Setup(IDXGIFactory4* factory)
{
    HRESULT hr = E_FAIL;

    // Get feature levels to try
    D3D_FEATURE_LEVEL feature_levels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
    };
    UINT num_feature_levels = ARRAYSIZE(feature_levels);

    // Try to find a GPU adapter that supports high-performance features
    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory_6;

    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(factory_6.GetAddressOf()))))
    {
        // Try to find a GPU adapter that supports high-performance features
        for 
        (
            UINT adapter_index = 0;

            // Get GPU function ride adapters one by one from the factory
            SUCCEEDED(factory_6->EnumAdapterByGpuPreference(
                adapter_index,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(adapter.GetAddressOf())));
            ++adapter_index
        ){
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            hr = E_FAIL;
            for (UINT feature_level_index = 0; feature_level_index < num_feature_levels; feature_level_index++)
            {
                feature_level_ = feature_levels[feature_level_index];
                if (SUCCEEDED(hr = D3D12CreateDevice(
                    adapter.Get(), feature_level_, _uuidof(ID3D12Device), &device_)))
                    break;
            }

            if (SUCCEEDED(hr))
                break;
        }
    }

    if (device_.Get() == nullptr)
    {
        // Find a GPU adapter that supports DirectX 12 features
        for (UINT adapter_index = 0; SUCCEEDED(factory->EnumAdapters1(adapter_index, &adapter)); ++adapter_index)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            hr = E_FAIL;
            for (UINT feature_level_index = 0; feature_level_index < num_feature_levels; feature_level_index++)
            {
                feature_level_ = feature_levels[feature_level_index];
                if (SUCCEEDED(hr = D3D12CreateDevice(
                    adapter.Get(), feature_level_, _uuidof(ID3D12Device), &device_)))
                    break;
            }

            if (SUCCEEDED(hr))
                break;
        }
    }

    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create D3D12 Device."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

#ifdef _DEBUG
    {
        ComPtr<ID3D12InfoQueue> infoQueue;
        if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue))))
        {
            // Break on error messages
            infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, TRUE);
        }
    }
#endif

    // Mark as created
    setup_flag_ = true;

    return true; // Success
}

ID3D12Device4* Device::Get()
{
    assert(setup_flag_); // Ensure the instance is created
    return device_.Get();
}

D3D_FEATURE_LEVEL Device::GetFeatureLevel() const
{
    assert(setup_flag_); // Ensure the instance is created
    return feature_level_;
}

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) :
    type_(type)
{
}

bool CommandQueue::Setup(ID3D12Device4 *device)
{
    // Create a command queue descriptor
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.Type = type_;

    // Create the command queue
    HRESULT hr = device->CreateCommandQueue(&desc, IID_PPV_ARGS(command_queue_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Command Queue."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Mark as created
    setup_flag_ = true;

    return true; // Success
}

ID3D12CommandQueue* CommandQueue::Get()
{
    assert(setup_flag_); // Ensure the instance is created
    return command_queue_.Get();
}

D3D12_COMMAND_LIST_TYPE CommandQueue::GetType() const
{
    assert(setup_flag_); // Ensure the instance is created
    return type_;
}

CommandAllocator::CommandAllocator(D3D12_COMMAND_LIST_TYPE type) :
    type_(type)
{
}

bool CommandAllocator::Setup(ID3D12Device4 *device)
{
    // Create the command allocator
    HRESULT hr = device->CreateCommandAllocator(type_, IID_PPV_ARGS(command_allocator_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Command Allocator."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    return true; // Success
}

ID3D12CommandAllocator *CommandAllocator::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return command_allocator_.Get();
}

D3D12_COMMAND_LIST_TYPE CommandAllocator::GetType()
{
    assert(IsSetup()); // Ensure the instance is created
    return type_;
}

CommandList::CommandList(CommandAllocator &allocator) :
    allocator_(allocator)
{
}

bool CommandList::Setup(ID3D12Device4 *device)
{
    // Create the command list
    HRESULT hr = device->CreateCommandList(
        0, allocator_.GetType(), allocator_.Get(), nullptr, IID_PPV_ARGS(command_list_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Command List."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Close the command list (it is created in the open state)
    hr = command_list_->Close();
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to close Command List after creation."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    return true; // Success
}

ID3D12GraphicsCommandList *CommandList::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return command_list_.Get();
}

CommandAllocator& CommandList::GetAllocator()
{
    assert(IsSetup()); // Ensure the instance is created
    return allocator_;
}

DescriptorHeap::DescriptorHeap(
    D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAGS flags) :
    type_(type),
    descriptor_count_(descriptor_count),
    flags_(flags)
{
}

bool DescriptorHeap::Setup(ID3D12Device4 *device)
{
    assert(descriptor_count_ > 0); // Ensure descriptor count is valid

    // Create descriptor heap description
    D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
    heap_desc.NumDescriptors = descriptor_count_;
    heap_desc.Type = type_;
    heap_desc.Flags = flags_;
    heap_desc.NodeMask = 0;

    // Create the descriptor heap
    HRESULT hr = device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&descriptor_heap_));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Descriptor Heap."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Get the descriptor handle increment size
    descriptor_handle_increment_ = device->GetDescriptorHandleIncrementSize(type_);

    return true; // Success
}

ID3D12DescriptorHeap *DescriptorHeap::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return descriptor_heap_.Get();
}

D3D12_DESCRIPTOR_HEAP_TYPE DescriptorHeap::GetType() const
{
    assert(IsSetup()); // Ensure the instance is created
    return type_;
}

UINT DescriptorHeap::GetDescriptorCount() const
{
    assert(IsSetup()); // Ensure the instance is created
    return descriptor_count_;
}

D3D12_DESCRIPTOR_HEAP_FLAGS DescriptorHeap::GetFlags() const
{
    assert(IsSetup()); // Ensure the instance is created
    return flags_;
}

UINT DescriptorHeap::GetDescriptorHandleIncrementSize() const
{
    assert(IsSetup()); // Ensure the instance is created
    return descriptor_handle_increment_;
}

DescriptorHeapAllocator::DescriptorHeapAllocator(DescriptorHeap &heap) :
    descriptor_heap_(heap)
{
}

bool DescriptorHeapAllocator::Setup(ID3D12Device4 *device)
{
    assert(descriptor_heap_.IsSetup()); // Ensure the descriptor heap is created

    // Get cpu handle start
    heap_start_cpu_handle_ = descriptor_heap_.Get()->GetCPUDescriptorHandleForHeapStart();

    // If the descriptor heap's flags indicate shader visibility, get gpu handle start
    if (descriptor_heap_.GetFlags() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
        heap_start_gpu_handle_ = descriptor_heap_.Get()->GetGPUDescriptorHandleForHeapStart();

    // Initialize the free indices stack
    free_indices_.reserve(descriptor_heap_.GetDescriptorCount());
    for (int n = descriptor_heap_.GetDescriptorCount(); n > 0; n--)
            free_indices_.push_back(n - 1);

    return true; // Success
}

void DescriptorHeapAllocator::Allocate(D3D12_CPU_DESCRIPTOR_HANDLE &cpu_handle)
{
    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(!free_indices_.empty()); // Ensure there are free descriptors

    // Get the index of the free descriptor
    UINT index = free_indices_.back();
    free_indices_.pop_back();

    // Calculate the CPU handle
    cpu_handle.ptr = heap_start_cpu_handle_.ptr + index * descriptor_heap_.GetDescriptorHandleIncrementSize();
}

void DescriptorHeapAllocator::Allocate(D3D12_GPU_DESCRIPTOR_HANDLE &gpu_handle)
{
    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(!free_indices_.empty()); // Ensure there are free descriptors
    assert(descriptor_heap_.GetFlags() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE); // Ensure the heap is shader visible

    // Get the index of the free descriptor
    UINT index = free_indices_.back();
    free_indices_.pop_back();

    // Calculate the GPU handle
    gpu_handle.ptr = heap_start_gpu_handle_.ptr + index * descriptor_heap_.GetDescriptorHandleIncrementSize();
}

void DescriptorHeapAllocator::Allocate(
    D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle)
{
    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(!free_indices_.empty()); // Ensure there are free descriptors
    assert(descriptor_heap_.GetFlags() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE); // Ensure the heap is shader visible

    // Get the index of the free descriptor
    UINT index = free_indices_.back();
    free_indices_.pop_back();

    // Calculate the CPU and GPU handles
    cpu_handle.ptr = heap_start_cpu_handle_.ptr + index * descriptor_heap_.GetDescriptorHandleIncrementSize();
    gpu_handle.ptr = heap_start_gpu_handle_.ptr + index * descriptor_heap_.GetDescriptorHandleIncrementSize();
}

void DescriptorHeapAllocator::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle)
{
    // Calculate the index from the CPU handle
    int cpu_index = (cpu_handle.ptr - heap_start_cpu_handle_.ptr) / descriptor_heap_.GetDescriptorHandleIncrementSize();

    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(cpu_index >= 0 && cpu_index < static_cast<int>(descriptor_heap_.GetDescriptorCount())); // Ensure the handle is valid
    assert(std::find(free_indices_.begin(), free_indices_.end(), cpu_index) == free_indices_.end()); // Ensure the handle is not already freed

    // Push the index back to the free indices stack
    free_indices_.push_back(cpu_index);
}

void DescriptorHeapAllocator::Free(D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
{
    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(descriptor_heap_.GetFlags() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE); // Ensure the heap is shader visible

    // Calculate the index from the GPU handle
    int gpu_index = (gpu_handle.ptr - heap_start_gpu_handle_.ptr) / descriptor_heap_.GetDescriptorHandleIncrementSize();
    assert(gpu_index >= 0 && gpu_index < static_cast<int>(descriptor_heap_.GetDescriptorCount())); // Ensure the handle is valid

    assert(std::find(free_indices_.begin(), free_indices_.end(), gpu_index) == free_indices_.end()); // Ensure the handle is not already freed

    // Push the index back to the free indices stack
    free_indices_.push_back(gpu_index);
}

void DescriptorHeapAllocator::Free(
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle)
{
    assert(IsSetup()); // Ensure the descriptor heap allocator is created
    assert(descriptor_heap_.GetFlags() == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE); // Ensure the heap is shader visible

    // Calculate the indices from the CPU and GPU handles
    int cpu_index = (cpu_handle.ptr - heap_start_cpu_handle_.ptr) / descriptor_heap_.GetDescriptorHandleIncrementSize();
    int gpu_index = (gpu_handle.ptr - heap_start_gpu_handle_.ptr) / descriptor_heap_.GetDescriptorHandleIncrementSize();

    assert(cpu_index >= 0 && cpu_index < static_cast<int>(descriptor_heap_.GetDescriptorCount())); // Ensure the CPU handle is valid
    assert(gpu_index >= 0 && gpu_index < static_cast<int>(descriptor_heap_.GetDescriptorCount())); // Ensure the GPU handle is valid
    assert(cpu_index == gpu_index); // Ensure the CPU and GPU handles correspond to the same descriptor

    assert(std::find(free_indices_.begin(), free_indices_.end(), cpu_index) == free_indices_.end()); // Ensure the handle is not already freed

    // Push the index back to the free indices stack
    free_indices_.push_back(cpu_index);
}

Fence::Fence(UINT64 initial_value, D3D12_FENCE_FLAGS flags, UINT timeout_ms) :
    current_value_(initial_value),
    flags_(flags),
    timeout_ms_(timeout_ms)
{
}

Fence::~Fence()
{
    // Close the fence event
    if (fence_event_ != nullptr)
    {
        CloseHandle(fence_event_);
        fence_event_ = nullptr;
    }
}

bool Fence::Setup(ID3D12Device4* device)
{
    // Create fence
    HRESULT hr = device->CreateFence(
        current_value_, flags_, IID_PPV_ARGS(fence_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Fence."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Create fence event
    fence_event_ = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    return true; // Success
}

ID3D12Fence* Fence::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return fence_.Get();
}

UINT64 Fence::GetCurrentValue() const
{
    assert(IsSetup()); // Ensure the instance is created
    return current_value_;
}

bool Fence::Signal(ID3D12CommandQueue* command_queue)
{
    assert(IsSetup()); // Ensure the instance is created

    HRESULT hr = E_FAIL;

    // Increment the fence value
    current_value_++;

    // Signal the fence on the command queue
    hr = command_queue->Signal(fence_.Get(), current_value_);
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to signal Fence."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Set a fence processing end confirmation event
    hr = fence_->SetEventOnCompletion(current_value_, fence_event_);

    // Store signaled value
    last_signaled_value_ = current_value_;

    return true; // Success
}

bool Fence::Wait()
{
    assert(IsSetup()); // Ensure the instance is created

    // Check if the fence has already reached the last signaled value
    if (fence_->GetCompletedValue() < last_signaled_value_)
    {
        // Wait for the fence event
        DWORD wait_result = WaitForSingleObject(fence_event_, timeout_ms_);

        // Check the wait result
        if (wait_result == WAIT_TIMEOUT)
        {
            utility_header::ConsoleLogErr(
                {"Timeout while waiting for Fence completion."}, E_FAIL, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }
        else if (wait_result != WAIT_OBJECT_0)
        {
            utility_header::ConsoleLogErr(
                {"Failed to wait for Fence completion."}, E_FAIL, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }
    }

    return true; // Success
}

SwapChain::SwapChain(
    UINT frame_count, UINT sync_interval, DXGI_SWAP_EFFECT swap_effect, DXGI_FORMAT format, std::wstring debug_name) :
    frame_count_(frame_count),
    sync_interval_(sync_interval),
    swap_effect_(swap_effect),
    format_(format),
    debug_name_(debug_name)
{
}

SwapChain::~SwapChain()
{
    if (!IsSetup())
        return;

    // Free descriptor heap
    for (auto& view : back_buffer_views_)
        back_buffer_heap_allocator_->Free(view);
}

bool SwapChain::Setup(
    ID3D12Device4 *device, IDXGIFactory4 *factory, ID3D12CommandQueue *command_queue, 
    HWND hwnd, UINT client_width, UINT client_height)
{
    HRESULT hr = E_FAIL;

    // Create the swap chain
    {
        // Describe and create the swap chain.
        DXGI_SWAP_CHAIN_DESC1 desc = {};
        desc.BufferCount = frame_count_;
        desc.Width = client_width;
        desc.Height = client_height;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SwapEffect = swap_effect_;
        desc.SampleDesc.Count = 1;

        // Create the swap chain1
        ComPtr<IDXGISwapChain1> swap_chain_1;
        hr = factory->CreateSwapChainForHwnd(
            command_queue, hwnd, &desc, nullptr, nullptr,& swap_chain_1);
        if (FAILED(hr))
        {
            utility_header::ConsoleLogErr(
                {"Failed to create Swap Chain."}, hr, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }

        // Cast to IDXGISwapChain3
        hr = swap_chain_1.As(&(swap_chain_));
        if (FAILED(hr))
        {
            utility_header::ConsoleLogErr(
                {"Failed to cast Swap Chain to IDXGISwapChain3."}, hr, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }
    }

    // Get the current back buffer index
    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    // Get the back buffers from the swap chain
    back_buffers_.clear();
    for (UINT i = 0; i < frame_count_; ++i)
    {
        // Get the i-th back buffer
        ComPtr<ID3D12Resource> buffer;
        hr = swap_chain_->GetBuffer(i, IID_PPV_ARGS(buffer.GetAddressOf()));
        if (FAILED(hr))
        {
            utility_header::ConsoleLogErr(
                {"Failed to get back buffer from Swap Chain."}, hr, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }

#ifdef _DEBUG
        // Set a debug name for the back buffer
        std::wstring buffer_name 
            = debug_name_ + L" Back Buffer " + std::to_wstring(client_width) + L"x" + std::to_wstring(client_height) 
            + L" [" + std::to_wstring(i) + L"]";
        buffer->SetName(buffer_name.c_str());
#endif

        // Store the buffer in the vector
        back_buffers_.push_back(buffer);
    }

    // Create descriptor heap for render target views (RTVs)
    back_buffer_heap_ = DescriptorHeap::CreateInstance<DescriptorHeap>(
        D3D12_DESCRIPTOR_HEAP_TYPE_RTV, frame_count_, D3D12_DESCRIPTOR_HEAP_FLAG_NONE, device);
    if (back_buffer_heap_ == nullptr)
        return false;

    // Create descriptor heap allocator for the RTV heap
    back_buffer_heap_allocator_ 
        = DescriptorHeapAllocator::CreateInstance<DescriptorHeapAllocator>(*back_buffer_heap_, device);
    if (back_buffer_heap_allocator_ == nullptr)
        return false;

    // Create render target views (RTVs) for the back buffers
    for (UINT i = 0; i < frame_count_; ++i)
    {
        // Allocate a descriptor for the back buffer
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
        back_buffer_heap_allocator_->Allocate(cpu_handle);

        // Create the RTV
        device->CreateRenderTargetView(back_buffers_[i].Get(), nullptr, cpu_handle);

        // Store the handles
        back_buffer_views_.push_back(cpu_handle);
    }

    // Create fence
    fence_ = Fence::CreateInstance<Fence>(fence_init_value_, D3D12_FENCE_FLAG_NONE, fence_timeout_ms_, device);
    if (fence_ == nullptr)
        return false; // Failed to create fence

    return true; // Success
}

ID3D12Resource* SwapChain::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return back_buffers_[frame_index_].Get();
}

ID3D12Resource* SwapChain::GetCurrentBackBuffer() const
{
    assert(IsSetup()); // Ensure the instance is created
    return back_buffers_[frame_index_].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCurrentBackBufferView() const
{
    assert(IsSetup()); // Ensure the instance is created
    return back_buffer_views_[frame_index_];
}

bool SwapChain::Resize(ID3D12Device4* device, UINT client_width, UINT client_height)
{
    assert(IsSetup()); // Ensure the instance is created

    HRESULT hr = E_FAIL;

    // Release the current back buffers
    back_buffers_.clear();

    // Release the current back buffer views
    for (UINT i = 0; i < frame_count_; ++i)
        back_buffer_heap_allocator_->Free(back_buffer_views_[i]);
    back_buffer_views_.clear();

    // Release the fences
    fence_.reset();
    
    // Resize the swap chain buffers
    hr = swap_chain_->ResizeBuffers(
        frame_count_, client_width, client_height, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to resize Swap Chain buffers."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Get the current back buffer index
    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    // Get the back buffers from the swap chain
    back_buffers_.clear();
    for (UINT i = 0; i < frame_count_; ++i)
    {
        // Get the i-th back buffer
        ComPtr<ID3D12Resource> buffer;
        hr = swap_chain_->GetBuffer(i, IID_PPV_ARGS(buffer.GetAddressOf()));
        if (FAILED(hr))
        {
            utility_header::ConsoleLogErr(
                {"Failed to get back buffer from Swap Chain."}, hr, __FILE__, __LINE__, __FUNCTION__);
            return false;
        }

        // Store the buffer in the vector
        back_buffers_.push_back(buffer);
    }

    // Create render target views (RTVs) for the back buffers
    for (UINT i = 0; i < frame_count_; ++i)
    {
        // Allocate a descriptor for the back buffer
        D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
        back_buffer_heap_allocator_->Allocate(cpu_handle);

        // Create the RTV
        device->CreateRenderTargetView(back_buffers_[i].Get(), nullptr, cpu_handle);

        // Store the handles
        back_buffer_views_.push_back(cpu_handle);
    }

    // Create fence
    fence_ = Fence::CreateInstance<Fence>(fence_init_value_, D3D12_FENCE_FLAG_NONE, fence_timeout_ms_, device);
    if (fence_ == nullptr)
        return false; // Failed to create fence

    return true; // Success
}

bool SwapChain::Present()
{
    assert(IsSetup()); // Ensure the instance is created

    HRESULT hr = E_FAIL;

    // Signal fence for the current frame
    if (!fence_->Signal(CommandQueue::GetInstance().Get()))
        return false; // Failed to signal fence

    // Present the swap chain
    hr = swap_chain_->Present(sync_interval_, 0);
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to present Swap Chain."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Update the frame index
    frame_index_ = swap_chain_->GetCurrentBackBufferIndex();

    return true; // Success
}

bool SwapChain::WaitForPreviousFrame(ID3D12CommandQueue* command_queue)
{
    assert(IsSetup()); // Ensure the instance is created

    // Wait for the previous frame to complete
    if (!fence_->Wait())
        return false; // Failed to wait for fence

    return true; // Success
}

UINT SwapChain::GetFrameIndex() const
{
    assert(IsSetup()); // Ensure the instance is created
    return frame_index_;
}

UINT SwapChain::GetFrameCount() const
{
    assert(IsSetup()); // Ensure the instance is created
    return frame_count_;
}

UINT SwapChain::GetSyncInterval() const
{
    assert(IsSetup()); // Ensure the instance is created
    return sync_interval_;
}

DXGI_SWAP_EFFECT SwapChain::GetSwapEffect() const
{
    assert(IsSetup()); // Ensure the instance is created
    return swap_effect_;
}

DXGI_FORMAT SwapChain::GetFormat() const
{
    assert(IsSetup()); // Ensure the instance is created
    return format_;
}

Buffer::Buffer(uint32_t size, D3D12_HEAP_TYPE heap_type, std::wstring debug_name) :
    size_(size),
    heap_type_(heap_type),
    debug_name_(debug_name)
{
}

Buffer::~Buffer()
{
    if (!IsSetup())
        return;

    // Free descriptor handles if allocated
    if (has_cpu_handle_)
        heap_allocator_->Free(cpu_handle_);

    if (has_gpu_handle_)
        heap_allocator_->Free(gpu_handle_);
}

bool Buffer::Setup(ID3D12Device4* device, DescriptorHeapAllocator* heap_allocator)
{
    // Store the heap allocator
    heap_allocator_ = heap_allocator;

    HRESULT hr = E_FAIL;

    CD3DX12_HEAP_PROPERTIES upheapProp = CD3DX12_HEAP_PROPERTIES(heap_type_);
    UINT fixedSize = (size_ + 255) & ~255;
    CD3DX12_RESOURCE_DESC sourceDesc = CD3DX12_RESOURCE_DESC::Buffer(fixedSize);

    hr = device->CreateCommittedResource(
        &upheapProp, D3D12_HEAP_FLAG_NONE,
        &sourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr, IID_PPV_ARGS(buffer_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Buffer."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

#ifdef _DEBUG
    // Set a name for the buffer for debugging purposes
    std::wstring buffer_name 
        = debug_name_ + L" Buffer " + std::to_wstring(size_) + L" bytes";
    buffer_->SetName(buffer_name.c_str());
#endif

    // Allocate descriptor handles if a heap allocator is provided
    if (heap_allocator != nullptr)
    {
        // Allocate CPU and GPU descriptor handles if a heap allocator is provided
        if (heap_type_ == D3D12_HEAP_TYPE_UPLOAD || heap_type_ == D3D12_HEAP_TYPE_READBACK)
        {
            // For upload and readback heaps, only allocate CPU handle
            heap_allocator->Allocate(cpu_handle_);
            has_cpu_handle_ = true;
            has_gpu_handle_ = false;
        }
        else
        {
            // For default heaps, allocate both CPU and GPU handles
            heap_allocator->Allocate(cpu_handle_, gpu_handle_);
            has_cpu_handle_ = true;
            has_gpu_handle_ = true;
        }
    }
    else // No heap allocator provided
    {
        has_cpu_handle_ = false;
        has_gpu_handle_ = false;
    }

    return true;
}

ID3D12Resource* Buffer::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return buffer_.Get();
}

ID3D12Resource* Buffer::Get() const
{
    assert(IsSetup()); // Ensure the instance is created
    return buffer_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Buffer::GetCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    assert(has_cpu_handle_); // Ensure the CPU handle is allocated
    return cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE Buffer::GetGpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    assert(has_gpu_handle_); // Ensure the GPU handle is allocated
    return gpu_handle_;
}

D3D12_GPU_VIRTUAL_ADDRESS Buffer::GetGPUVirtualAddress() const
{
    assert(IsSetup()); // Ensure the instance is created
    return buffer_->GetGPUVirtualAddress();
}

uint32_t Buffer::GetSize() const
{
    assert(IsSetup()); // Ensure the instance is created
    return size_;
}

D3D12_HEAP_TYPE Buffer::GetHeapType() const
{
    assert(IsSetup()); // Ensure the instance is created
    return heap_type_;
}

bool Buffer::UpdateData(const void *data, uint32_t size)
{
    HRESULT hr = E_FAIL;

    // Map the buffer
    uint8_t* memory = nullptr;
    hr = buffer_->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to map Buffer."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Copy the initial data to the buffer
    std::memcpy(memory, data, size);

    // Unmap the buffer
    buffer_->Unmap(0, nullptr);

    return true; // Success
}

bool Buffer::UpdateData(const void* data, uint32_t size, uint32_t offset)
{
    HRESULT hr = E_FAIL;

    // Map the buffer
    uint8_t* memory = nullptr;
    hr = buffer_->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to map Buffer."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

    // Copy the initial data to the buffer at the specified offset
    std::memcpy(memory + offset, data, size);

    // Unmap the buffer
    buffer_->Unmap(0, nullptr);

    return true; // Success
}

StructuredBuffer::StructuredBuffer(
    uint32_t element_size, uint32_t element_count, D3D12_HEAP_TYPE heap_type, bool uav_enable, std::wstring debug_name) :
    element_size_(element_size),
    element_count_(element_count),
    heap_type_(heap_type),
    uav_enable_(uav_enable),
    debug_name_(debug_name)
{
}

StructuredBuffer::~StructuredBuffer()
{
    if (!IsSetup())
        return;

    // Free descriptor handles
    heap_allocator_->Free(srv_cpu_handle_, srv_gpu_handle_);

    if (uav_enable_)
        heap_allocator_->Free(uav_cpu_handle_, uav_gpu_handle_);

}

bool StructuredBuffer::Setup(ID3D12Device4* device, DescriptorHeapAllocator* heap_allocator)
{
    // Store the heap allocator
    heap_allocator_ = heap_allocator;

    HRESULT hr = E_FAIL;

    // Calculate buffer size and align to 256 bytes
    UINT64 buffer_size = static_cast<UINT64>(element_size_) * static_cast<UINT64>(element_count_);
    UINT fixedSize = static_cast<UINT>((buffer_size + 255) & ~255);

    CD3DX12_HEAP_PROPERTIES upheapProp = CD3DX12_HEAP_PROPERTIES(heap_type_);
    CD3DX12_RESOURCE_DESC sourceDesc = CD3DX12_RESOURCE_DESC::Buffer(fixedSize);

    // Create the buffer resource
    hr = device->CreateCommittedResource(
        &upheapProp, D3D12_HEAP_FLAG_NONE,
        &sourceDesc, D3D12_RESOURCE_STATE_COMMON,
        nullptr, IID_PPV_ARGS(buffer_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create StructuredBuffer."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

#ifdef _DEBUG
    // Set a name for the buffer for debugging purposes
    std::wstring buffer_name 
        = debug_name_ + L" StructuredBuffer " + std::to_wstring(element_size_) + L" bytes x " + std::to_wstring(element_count_);
    buffer_->SetName(buffer_name.c_str());
#endif

    // Set current state
	current_state_ = D3D12_RESOURCE_STATE_COMMON;

    // Allocate SRV descriptor
    assert(heap_allocator != nullptr); // Ensure SRV heap allocator is provided
    heap_allocator->Allocate(srv_cpu_handle_, srv_gpu_handle_);

    // Create SRV
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = DXGI_FORMAT_UNKNOWN;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
        srv_desc.Buffer.FirstElement = 0;
        srv_desc.Buffer.NumElements = element_count_;
        srv_desc.Buffer.StructureByteStride = element_size_;
        srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

        device->CreateShaderResourceView(buffer_.Get(), &srv_desc, srv_cpu_handle_);
    }

    if (uav_enable_)
    {
        // Allocate UAV descriptor
        assert(heap_allocator != nullptr); // Ensure UAV heap allocator is provided
        heap_allocator->Allocate(uav_cpu_handle_, uav_gpu_handle_);

        // Create UAV
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = DXGI_FORMAT_UNKNOWN;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
        uav_desc.Buffer.FirstElement = 0;
        uav_desc.Buffer.NumElements = element_count_;
        uav_desc.Buffer.StructureByteStride = element_size_;
        uav_desc.Buffer.CounterOffsetInBytes = 0;
        uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

        device->CreateUnorderedAccessView(buffer_.Get(), nullptr, &uav_desc, uav_cpu_handle_);
    }

    return true; // Success
}

ID3D12Resource* StructuredBuffer::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return buffer_.Get();
}

ID3D12Resource* StructuredBuffer::Get() const
{
    assert(IsSetup()); // Ensure the instance is created
    return buffer_.Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE StructuredBuffer::GetSrvCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return srv_cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE StructuredBuffer::GetSrvGpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return srv_gpu_handle_;
}

D3D12_CPU_DESCRIPTOR_HANDLE StructuredBuffer::GetUavCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    assert(uav_enable_); // Ensure UAV is enabled
    return uav_cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE StructuredBuffer::GetUavGpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    assert(uav_enable_); // Ensure UAV is enabled
    return uav_gpu_handle_;
}

Texture2D::Texture2D(
    UINT width, UINT height, DXGI_FORMAT format, 
    D3D12_HEAP_TYPE heap_type, D3D12_RESOURCE_FLAGS resource_flags, D3D12_RESOURCE_STATES initial_state,
    std::wstring debug_name) :
    width_(width),
    height_(height),
    format_(format),
    heap_type_(heap_type),
    resource_flags_(resource_flags),
    current_state_(initial_state),
    debug_name_(debug_name)
{
}

Texture2D::~Texture2D()
{
    if (!IsSetup())
        return;

    // Free descriptor handles
    srv_heap_allocator_->Free(srv_cpu_handle_, srv_gpu_handle_);

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
    {
        if (srv_heap_allocator_ != nullptr)
            srv_heap_allocator_->Free(uav_cpu_handle_, uav_gpu_handle_);
    }

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
    {
        if (rtv_heap_allocator_ != nullptr)
            rtv_heap_allocator_->Free(rtv_cpu_handle_);
    }

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
    {
        if (dsv_heap_allocator_ != nullptr)
            dsv_heap_allocator_->Free(dsv_cpu_handle_);
    }
}

bool Texture2D::Setup(
    ID3D12Device4* device, D3D12_CLEAR_VALUE* clear_value, 
    const DXGI_FORMAT* srv_format, const DXGI_FORMAT* uav_format, 
    const DXGI_FORMAT* rtv_format, const DXGI_FORMAT* dsv_format,
    DescriptorHeapAllocator* srv_heap_allocator,
    DescriptorHeapAllocator* rtv_heap_allocator,
    DescriptorHeapAllocator* dsv_heap_allocator)
{
    // Store the heap allocators
    srv_heap_allocator_ = srv_heap_allocator;
    rtv_heap_allocator_ = rtv_heap_allocator;
    dsv_heap_allocator_ = dsv_heap_allocator;

    HRESULT hr = E_FAIL;

    // Define texture description
    const UINT16 ARRAY_SIZE = 1;
    const UINT16 MIP_LEVELS = 0;
    const UINT SAMPLE_COUNT = 1;
    const UINT SAMPLE_QUALITY = 0;
    CD3DX12_RESOURCE_DESC texture_desc 
        = CD3DX12_RESOURCE_DESC::Tex2D(
            format_, width_, height_, 
            ARRAY_SIZE, MIP_LEVELS, SAMPLE_COUNT, SAMPLE_QUALITY, resource_flags_, D3D12_TEXTURE_LAYOUT_UNKNOWN);

    // Define heap properties
    CD3DX12_HEAP_PROPERTIES heap_properties = CD3DX12_HEAP_PROPERTIES(heap_type_);

    hr = device->CreateCommittedResource(
        &heap_properties, D3D12_HEAP_FLAG_NONE,
        &texture_desc, current_state_,
        clear_value, IID_PPV_ARGS(texture_.GetAddressOf()));
    if (FAILED(hr))
    {
        utility_header::ConsoleLogErr(
            {"Failed to create Texture2D."}, hr, __FILE__, __LINE__, __FUNCTION__);
        return false;
    }

#ifdef _DEBUG
    // Set a name for the texture for debugging purposes
    std::wstring texture_name 
        = debug_name_ + L" Texture2D " + std::to_wstring(width_) + L"x" + std::to_wstring(height_);
    texture_->SetName(texture_name.c_str());
#endif

    // Allocate SRV descriptor
    assert(srv_heap_allocator != nullptr); // Ensure SRV heap allocator is provided
    srv_heap_allocator->Allocate(srv_cpu_handle_, srv_gpu_handle_);

    // Create SRV
    {
        D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
        srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srv_desc.Format = (srv_format != nullptr) ? *srv_format : format_;
        srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srv_desc.Texture2D.MipLevels = 1;

        device->CreateShaderResourceView(texture_.Get(), &srv_desc, srv_cpu_handle_);
    }

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS)
    {
        // Allocate UAV descriptor
        assert(srv_heap_allocator != nullptr); // Ensure SRV heap allocator is provided
        srv_heap_allocator->Allocate(uav_cpu_handle_, uav_gpu_handle_);

        // Create UAV
        D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
        uav_desc.Format = (uav_format != nullptr) ? *uav_format : format_;
        uav_desc.ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D;
        uav_desc.Texture2D.MipSlice = 0;

        device->CreateUnorderedAccessView(texture_.Get(), nullptr, &uav_desc, uav_cpu_handle_);
    }

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET)
    {
        // Allocate RTV descriptor
        assert(rtv_heap_allocator != nullptr); // Ensure RTV heap allocator is provided
        rtv_heap_allocator->Allocate(rtv_cpu_handle_);

        // Create RTV
        D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
        rtv_desc.Format = (rtv_format != nullptr) ? *rtv_format : format_;
        rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtv_desc.Texture2D.MipSlice = 0;
        device->CreateRenderTargetView(texture_.Get(), &rtv_desc, rtv_cpu_handle_);
    }

    if (resource_flags_ & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL)
    {
        // Allocate DSV descriptor
        assert(dsv_heap_allocator != nullptr); // Ensure DSV heap allocator is provided
        dsv_heap_allocator->Allocate(dsv_cpu_handle_);

        // Create DSV
        D3D12_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
        dsv_desc.Format = (dsv_format != nullptr) ? *dsv_format : format_;
        dsv_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsv_desc.Flags = D3D12_DSV_FLAG_NONE;
        dsv_desc.Texture2D.MipSlice = 0;
        device->CreateDepthStencilView(texture_.Get(), &dsv_desc, dsv_cpu_handle_);
    }

    return true; // Success
}

ID3D12Resource* Texture2D::Get()
{
    assert(IsSetup()); // Ensure the instance is created
    return texture_.Get();
}

D3D12_RESOURCE_STATES Texture2D::GetCurrentState() const
{
    assert(IsSetup()); // Ensure the instance is created
    return current_state_;
}

void Texture2D::SetCurrentState(D3D12_RESOURCE_STATES state)
{
    assert(IsSetup()); // Ensure the instance is created
    current_state_ = state;
}


D3D12_CPU_DESCRIPTOR_HANDLE Texture2D::GetSrvCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return srv_cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture2D::GetSrvGpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return srv_gpu_handle_;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture2D::GetUavCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return uav_cpu_handle_;
}

D3D12_GPU_DESCRIPTOR_HANDLE Texture2D::GetUavGpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return uav_gpu_handle_;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture2D::GetRtvCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return rtv_cpu_handle_;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture2D::GetDsvCpuHandle() const
{
    assert(IsSetup()); // Ensure the instance is created
    return dsv_cpu_handle_;
}

UINT Texture2D::GetWidth() const
{
    return width_;
}

UINT Texture2D::GetHeight() const
{
    return height_;
}

DXGI_FORMAT Texture2D::GetFormat() const
{
    return format_;
}

D3D12_HEAP_TYPE Texture2D::GetHeapType() const
{
    return heap_type_;
}

D3D12_RESOURCE_FLAGS Texture2D::GetResourceFlags() const
{
    return resource_flags_;
}

Barrier::Barrier(
    ID3D12Resource* resource, ID3D12GraphicsCommandList* command_list, 
    const D3D12_RESOURCE_STATES& before_state, const D3D12_RESOURCE_STATES& after_state, UINT num_barriers)
{
    assert(resource != nullptr); // Ensure the resource is valid
    assert(command_list != nullptr); // Ensure the command list is valid
    assert(before_state != after_state); // Ensure the states are different

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, before_state, after_state);
    command_list->ResourceBarrier(num_barriers, &barrier);
}

} // namespace dx12_util