#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <vector>
#include <memory>
#include <string>

#include "class_template/singleton.h"
#include "class_template/instance.h"
#include "directx12_util/include/dll_config.h" // DLL export/import macros

namespace dx12_util
{

// DX12 factory wrapper class
class DX12_UTIL_DLL DXFactory :
    public class_template::Singleton<DXFactory>
{
public:
    DXFactory() = default;
    ~DXFactory() override = default;
    bool Setup();

    IDXGIFactory4* Get();

private:
    bool setup_flag_ = false;
    Microsoft::WRL::ComPtr<IDXGIFactory4> factory_ = nullptr;
};

// DirectX 12 device wrapper class
class DX12_UTIL_DLL Device :
    public class_template::Singleton<Device>
{
public:
    Device() = default;
    ~Device() override = default;
    bool Setup(IDXGIFactory4* factory);

    ID3D12Device4* Get();
    D3D_FEATURE_LEVEL GetFeatureLevel() const;

private:
    bool setup_flag_ = false;
    Microsoft::WRL::ComPtr<ID3D12Device4> device_ = nullptr;
    D3D_FEATURE_LEVEL feature_level_ = D3D_FEATURE_LEVEL_12_0;
};

// Command queue wrapper class
class DX12_UTIL_DLL CommandQueue :
    public class_template::Singleton<CommandQueue>
{
public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
    ~CommandQueue() override = default;
    bool Setup(ID3D12Device4* device);

    ID3D12CommandQueue* Get();
    D3D12_COMMAND_LIST_TYPE GetType() const;

private:
    bool setup_flag_ = false;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> command_queue_ = nullptr;
    const D3D12_COMMAND_LIST_TYPE type_;
};

// Command allocator wrapper class
class DX12_UTIL_DLL CommandAllocator :
    public class_template::InstanceGuard<
        CommandAllocator,
        class_template::ConstructArgList<D3D12_COMMAND_LIST_TYPE>,
        class_template::SetupArgList<ID3D12Device4*>>
{
public:
    CommandAllocator(D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
    ~CommandAllocator() override = default;
    virtual bool Setup(ID3D12Device4* device) override;

    ID3D12CommandAllocator* Get();
    D3D12_COMMAND_LIST_TYPE GetType();

private:
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> command_allocator_ = nullptr;
    const D3D12_COMMAND_LIST_TYPE type_;
};

// Command list wrapper class
class DX12_UTIL_DLL CommandList :
    public class_template::InstanceGuard<
        CommandList,
        class_template::ConstructArgList<CommandAllocator&>,
        class_template::SetupArgList<ID3D12Device4*>>
{
public:
    CommandList(CommandAllocator& allocator);
    ~CommandList() override = default;
    virtual bool Setup(ID3D12Device4* device) override;

    ID3D12GraphicsCommandList* Get();
    CommandAllocator& GetAllocator();

private:
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> command_list_ = nullptr;
    CommandAllocator& allocator_;
};

// Descriptor heap wrapper class
class DX12_UTIL_DLL DescriptorHeap : 
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        DescriptorHeap,
        class_template::ConstructArgList<D3D12_DESCRIPTOR_HEAP_TYPE, UINT, D3D12_DESCRIPTOR_HEAP_FLAGS>,
        class_template::SetupArgList<ID3D12Device4*>>
{
public:
    DescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT descriptor_count, D3D12_DESCRIPTOR_HEAP_FLAGS flags);
    ~DescriptorHeap() override = default;
    virtual bool Setup(ID3D12Device4* device) override;

    ID3D12DescriptorHeap* Get();
    D3D12_DESCRIPTOR_HEAP_TYPE GetType() const;
    UINT GetDescriptorCount() const;
    D3D12_DESCRIPTOR_HEAP_FLAGS GetFlags() const;
    UINT GetDescriptorHandleIncrementSize() const;

private:
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptor_heap_ = nullptr;
    const D3D12_DESCRIPTOR_HEAP_TYPE type_;
    const UINT descriptor_count_;
    const D3D12_DESCRIPTOR_HEAP_FLAGS flags_;
    UINT descriptor_handle_increment_;
};

// A descriptor heap allocator for managing CPU and GPU descriptor handles
// This class provides methods to allocate and free descriptors in a descriptor heap
class DX12_UTIL_DLL DescriptorHeapAllocator : 
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        DescriptorHeapAllocator,
        class_template::ConstructArgList<DescriptorHeap&>,
        class_template::SetupArgList<ID3D12Device4*>>
{
public:
    DescriptorHeapAllocator(DescriptorHeap& heap);
    ~DescriptorHeapAllocator() override = default;
    virtual bool Setup(ID3D12Device4* device) override;

    // Allocate a descriptor and return its handle
    // Throws std::runtime_error if allocation fails
    void Allocate(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle);
    void Allocate(D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle);
    void Allocate(D3D12_CPU_DESCRIPTOR_HANDLE& cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE& gpu_handle);

    // Free a previously allocated descriptor by its handle
    // Throws std::runtime_error if the handle is invalid
    void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle);
    void Free(D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle);
    void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle);

private:
    DescriptorHeap& descriptor_heap_; // Reference to the descriptor heap
    D3D12_CPU_DESCRIPTOR_HANDLE heap_start_cpu_handle_ = {}; // Start CPU handle of the heap
    D3D12_GPU_DESCRIPTOR_HANDLE heap_start_gpu_handle_ = {}; // Start GPU handle of the heap

    std::vector<UINT> free_indices_; // Stack of free descriptor indices
};

// Fence wrapper class
class DX12_UTIL_DLL Fence :
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        Fence,
        class_template::ConstructArgList<UINT64, D3D12_FENCE_FLAGS, UINT>,
        class_template::SetupArgList<ID3D12Device4*>>
{
public:
    Fence(UINT64 initial_value, D3D12_FENCE_FLAGS flags, UINT timeout_ms);
    ~Fence() override;
    virtual bool Setup(ID3D12Device4* device) override;

    ID3D12Fence* Get();
    UINT64 GetCurrentValue() const;

    // Signal the fence from the command queue
    // If signaling fails, return false
    bool Signal(ID3D12CommandQueue* command_queue);

    // Wait for the fence to reach the current value
    // If waiting fails, return false
    bool Wait();

private:
    // Underlying D3D12 fence
    Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;

    // Current fence value
    UINT64 current_value_ = 0;

    // Last signaled value
    UINT64 last_signaled_value_ = 0;

    // Fence flags
    D3D12_FENCE_FLAGS flags_;

    // Timeout for waiting
    const UINT timeout_ms_;

    // Fence event handle
    HANDLE fence_event_ = nullptr;
};

// Base resource wrapper class
class DX12_UTIL_DLL Resource
{
public:
    Resource() = default;
    virtual ~Resource() = default;

    // Get the underlying ID3D12Resource pointer
    virtual ID3D12Resource* Get() = 0;
};

// Swap chain wrapper class
class DX12_UTIL_DLL SwapChain : 
    public Resource,
    public class_template::NonCopyable,
    public class_template::InstanceGuard<
        SwapChain,
        class_template::ConstructArgList<UINT, UINT, DXGI_SWAP_EFFECT, DXGI_FORMAT, std::wstring>,
        class_template::SetupArgList<ID3D12Device4*, IDXGIFactory4*, ID3D12CommandQueue*, HWND, UINT, UINT>>
{
public:
    SwapChain(
        UINT frame_count = 2, UINT sync_interval = 1, 
        DXGI_SWAP_EFFECT swap_effect = DXGI_SWAP_EFFECT_FLIP_DISCARD, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM,
        std::wstring debug_name = L"");
    ~SwapChain() override;
    virtual bool Setup(
        ID3D12Device4* device, IDXGIFactory4* factory, ID3D12CommandQueue* command_queue, 
        HWND hwnd, UINT client_width, UINT client_height) override;
    virtual ID3D12Resource* Get() override;

    // Get the current back buffer resource
    ID3D12Resource* GetCurrentBackBuffer() const;

    // Get the CPU descriptor handle of the current back buffer
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;

    // Resize the swap chain buffers
    // If resizing fails, return false
    bool Resize(ID3D12Device4* device, UINT client_width, UINT client_height);

    // Present the swap chain
    // If presenting fails, return false
    bool Present();

    // Wait for the previous frame to finish
    // This function uses a fence to synchronize the CPU and GPU
    // If waiting fails, return false
    bool WaitForPreviousFrame(ID3D12CommandQueue* command_queue);

    UINT GetFrameIndex() const;
    UINT GetFrameCount() const;
    UINT GetSyncInterval() const;
    DXGI_SWAP_EFFECT GetSwapEffect() const;
    DXGI_FORMAT GetFormat() const;

private:
    Microsoft::WRL::ComPtr<IDXGISwapChain3> swap_chain_ = nullptr;
    UINT frame_count_ = 0;
    UINT frame_index_ = 0;
    UINT sync_interval_ = 0;
    DXGI_SWAP_EFFECT swap_effect_ = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

    std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> back_buffers_;
    std::unique_ptr<DescriptorHeap> back_buffer_heap_ = nullptr;
    std::unique_ptr<DescriptorHeapAllocator> back_buffer_heap_allocator_ = nullptr;
    std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> back_buffer_views_;

    // Initial value of the fence
    const UINT64 fence_init_value_ = 0;
    const UINT fence_timeout_ms_ = 100000;

    // Debug name
    const std::wstring debug_name_;

    // Fence for synchronization
    std::unique_ptr<Fence> fence_;
};

// Buffer wrapper class
class DX12_UTIL_DLL Buffer :
    public Resource,
    public class_template::InstanceGuard<
        Buffer,
        class_template::ConstructArgList<uint32_t, D3D12_HEAP_TYPE, std::wstring>,
        class_template::SetupArgList<ID3D12Device4*, DescriptorHeapAllocator*>>
{
public:
    Buffer(uint32_t size, D3D12_HEAP_TYPE heap_type, std::wstring debug_name = L"");
    ~Buffer() override;
    virtual bool Setup(
        ID3D12Device4* device, DescriptorHeapAllocator* heap_allocator = nullptr) override;
    ID3D12Resource* Get() override;

    // Get the underlying ID3D12Resource pointer (const version)
    ID3D12Resource* Get() const;

    // Get the CPU descriptor handle (if created)
    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const;

    // Get the GPU descriptor handle (if created)
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const;

    // Get the GPU virtual address of the buffer
    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

    // Get the size of the buffer
    uint32_t GetSize() const;

    // Get the heap type of the buffer
    D3D12_HEAP_TYPE GetHeapType() const;

    // Update the buffer data (only for upload heap)
    bool UpdateData(const void* data, uint32_t size);

    // Update the buffer data at the specified offset (only for upload heap)
    bool UpdateData(const void* data, uint32_t size, uint32_t offset);

private:
    // Descriptor heap allocator (optional)
    DescriptorHeapAllocator* heap_allocator_ = nullptr;

    // Underlying D3D12 resource
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer_ = nullptr;

    // CPU descriptor handle
    bool has_cpu_handle_ = false;
    D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle_ = {};

    // GPU descriptor handle
    bool has_gpu_handle_ = false;
    D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle_ = {};

    // Buffer size
    const uint32_t size_;

    // Heap type
    const D3D12_HEAP_TYPE heap_type_;

    // Debug name
    const std::wstring debug_name_;
};

class DX12_UTIL_DLL StructuredBuffer :
    public Resource,
    public class_template::InstanceGuard<
        StructuredBuffer,
        class_template::ConstructArgList<uint32_t, uint32_t, D3D12_HEAP_TYPE, bool, std::wstring>,
        class_template::SetupArgList<ID3D12Device4*, DescriptorHeapAllocator*>>
{
public:
    StructuredBuffer(
        uint32_t element_size, uint32_t element_count, D3D12_HEAP_TYPE heap_type, 
        bool uav_enable, std::wstring debug_name = L"");
    ~StructuredBuffer() override;
    virtual bool Setup(
        ID3D12Device4* device, DescriptorHeapAllocator* heap_allocator) override;
    ID3D12Resource* Get() override;

    // Get the underlying ID3D12Resource pointer (const version)
    ID3D12Resource* Get() const;

    // Get the SRV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle() const;

    // Get the SRV GPU descriptor handle
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() const;

    // Get the UAV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetUavCpuHandle() const;

    // Get the UAV GPU descriptor handle
    D3D12_GPU_DESCRIPTOR_HANDLE GetUavGpuHandle() const;

    // Get the size of each element
    const uint32_t GetElementSize() const { return element_size_; }

    // Get the number of elements
    const uint32_t GetElementCount() const { return element_count_; }

    // Get the heap type of the buffer
    D3D12_HEAP_TYPE GetHeapType() const { return heap_type_; }

    // Get whether UAV is enabled
    bool IsUavEnabled() const { return uav_enable_; }

    // Get the current resource state
    D3D12_RESOURCE_STATES GetCurrentState() const { return current_state_; }

    // Set the current resource state
    void SetCurrentState(D3D12_RESOURCE_STATES state) { current_state_ = state; }

private:
    // Descriptor heap allocator
    DescriptorHeapAllocator* heap_allocator_ = nullptr;

    // Underlying D3D12 resource
    Microsoft::WRL::ComPtr<ID3D12Resource> buffer_ = nullptr;

    // Size of each element in the buffer
    const uint32_t element_size_;

    // Number of elements in the buffer
    const uint32_t element_count_;

    // Heap type of the buffer
    const D3D12_HEAP_TYPE heap_type_;

    // Whether UAV is enabled
    const bool uav_enable_;

    // Debug name
    const std::wstring debug_name_;

    // SRV descriptor handles
    D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle_ = {};

    // UAV descriptor handles
    D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_handle_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_handle_ = {};

    // Current resource state
    D3D12_RESOURCE_STATES current_state_;
};

class DX12_UTIL_DLL Texture2D :
    public Resource,
    public class_template::InstanceGuard<
        Texture2D,
        class_template::ConstructArgList<
            UINT, UINT, DXGI_FORMAT, D3D12_HEAP_TYPE, D3D12_RESOURCE_FLAGS, D3D12_RESOURCE_STATES, std::wstring>,
        class_template::SetupArgList<
            ID3D12Device4*, D3D12_CLEAR_VALUE*, 
            const DXGI_FORMAT*, const DXGI_FORMAT*, const DXGI_FORMAT*, const DXGI_FORMAT*, 
            DescriptorHeapAllocator*, DescriptorHeapAllocator*, DescriptorHeapAllocator*>>
{
public:
    Texture2D(
        UINT width, UINT height, DXGI_FORMAT format,
        D3D12_HEAP_TYPE heap_type, D3D12_RESOURCE_FLAGS resource_flags, D3D12_RESOURCE_STATES initial_state, 
        std::wstring debug_name = L"");
    ~Texture2D() override;
    virtual bool Setup(
        ID3D12Device4* device, D3D12_CLEAR_VALUE* clear_value,
        const DXGI_FORMAT* srv_format, // Optional formats for creating view, If nullptr, use texture format
        const DXGI_FORMAT* uav_format, // Optional formats for creating views, If nullptr, use texture format
        const DXGI_FORMAT* rtv_format, // Optional formats for creating views, If nullptr, use texture format
        const DXGI_FORMAT* dsv_format,// Optional formats for creating views, If nullptr, use texture format
        DescriptorHeapAllocator* srv_heap_allocator, // Optional SRV heap allocator, If nullptr, SRV is not created
        DescriptorHeapAllocator* rtv_heap_allocator, // Optional RTV heap allocator, If nullptr, RTV is not created
        DescriptorHeapAllocator* dsv_heap_allocator) override; // Optional DSV heap allocator, If nullptr, DSV is not created
    ID3D12Resource* Get() override;

    // Get the SRV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetSrvCpuHandle() const;

    // Get the SRV GPU descriptor handle
    D3D12_GPU_DESCRIPTOR_HANDLE GetSrvGpuHandle() const;

    // Get the UAV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetUavCpuHandle() const;

    // Get the UAV GPU descriptor handle
    D3D12_GPU_DESCRIPTOR_HANDLE GetUavGpuHandle() const;

    // Get the RTV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetRtvCpuHandle() const;

    // Get the DSV CPU descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE GetDsvCpuHandle() const;

    // Get the texture width
    UINT GetWidth() const;

    // Get the texture height
    UINT GetHeight() const;

    // Get the texture format
    DXGI_FORMAT GetFormat() const;

    // Get the heap type of the texture
    D3D12_HEAP_TYPE GetHeapType() const;

    // Get the resource flags of the texture
    D3D12_RESOURCE_FLAGS GetResourceFlags() const;

    // Get the current resource state
    D3D12_RESOURCE_STATES GetCurrentState() const;

    // Set the current resource state
    void SetCurrentState(D3D12_RESOURCE_STATES state);

private:
    // Descriptor heap allocators
    DescriptorHeapAllocator* srv_heap_allocator_ = nullptr;
    DescriptorHeapAllocator* rtv_heap_allocator_ = nullptr;
    DescriptorHeapAllocator* dsv_heap_allocator_ = nullptr;

    // Underlying D3D12 resource
    Microsoft::WRL::ComPtr<ID3D12Resource> texture_ = nullptr;

    // SRV descriptor handles
    D3D12_CPU_DESCRIPTOR_HANDLE srv_cpu_handle_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE srv_gpu_handle_ = {};

    // UAV descriptor handles
    D3D12_CPU_DESCRIPTOR_HANDLE uav_cpu_handle_ = {};
    D3D12_GPU_DESCRIPTOR_HANDLE uav_gpu_handle_ = {};

    // RTV descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE rtv_cpu_handle_ = {};

    // DSV descriptor handle
    D3D12_CPU_DESCRIPTOR_HANDLE dsv_cpu_handle_ = {};

    // Width of the texture
    const UINT width_;

    // Height of the texture
    const UINT height_;

    // Format of the texture
    const DXGI_FORMAT format_;

    // Heap type of the texture
    const D3D12_HEAP_TYPE heap_type_;

    // Resource flags of the texture
    const D3D12_RESOURCE_FLAGS resource_flags_;

    // Debug name
    const std::wstring debug_name_;

    // Current resource state
    D3D12_RESOURCE_STATES current_state_;
};

// Resource barrier wrapper class
class DX12_UTIL_DLL Barrier 
{
public:
    Barrier(
        ID3D12Resource* resource, ID3D12GraphicsCommandList* command_list,
        const D3D12_RESOURCE_STATES& before_state, const D3D12_RESOURCE_STATES& after_state, UINT num_barriers = 1);
    virtual ~Barrier() = default;
};

} // namespace dx12_util