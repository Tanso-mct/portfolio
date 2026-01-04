#include "wbp_d3d12/src/pch.h"
#include "wbp_d3d12/include/d3d12_helpers.h"

#pragma comment(lib, "windows_base.lib")

using Microsoft::WRL::ComPtr;

#include "directx12_helper/include/d3dx12.h"
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

WBP_D3D12_API void wbp_d3d12::CreateDX12Factory(ComPtr<IDXGIFactory4> &factory)
{
    UINT dxgiFactoryFlags = 0;

#ifndef NDEBUG
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(debugController.GetAddressOf()))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layer features
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;

            ComPtr<ID3D12Debug1> spDebugController1;
            debugController->QueryInterface(IID_PPV_ARGS(spDebugController1.GetAddressOf()));
            spDebugController1->SetEnableGPUBasedValidation(true);
        }
    }
#endif

    HRESULT hr = CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(factory.GetAddressOf()));
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create DXGI Factory: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateDX12Device
(
    ComPtr<ID3D12Device4> &device, D3D_FEATURE_LEVEL &dstFeatureLevel, 
    ComPtr<IDXGIFactory4> &factory
){
    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_12_2,
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;
    HRESULT hr = E_FAIL;

    if (SUCCEEDED(factory->QueryInterface(IID_PPV_ARGS(factory6.GetAddressOf()))))
    {
        // Try to find a GPU adapter that supports high-performance features
        for (
            UINT adapterIndex = 0;

            // Get GPU function ride adapters one by one from the factory
            SUCCEEDED(factory6->EnumAdapterByGpuPreference
            (
                adapterIndex,
                DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
                IID_PPV_ARGS(adapter.GetAddressOf())
            ));
            ++adapterIndex
        ){
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            hr = E_FAIL;
            for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
            {
                dstFeatureLevel = featureLevels[featureLevel];
                if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), dstFeatureLevel, _uuidof(ID3D12Device), &device)))
                {
                    break;
                }
            }

            if (SUCCEEDED(hr))
                break;
        }
    }

    if (device.Get() == nullptr)
    {
        // Find a GPU adapter that supports DirectX 12 features
        for (UINT adapterIndex = 0; SUCCEEDED(factory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;

            hr = E_FAIL;
            for (UINT featureLevel = 0; featureLevel < numFeatureLevels; featureLevel++)
            {
                dstFeatureLevel = featureLevels[featureLevel];
                if (SUCCEEDED(hr = D3D12CreateDevice(adapter.Get(), dstFeatureLevel, _uuidof(ID3D12Device), &device)))
                {
                    break;
                }

            }

            if (SUCCEEDED(hr))
                break;
        }
    }

    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Device: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateDX12CommandQueue
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12CommandQueue> &commandQueue
){
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));

    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Command Queue: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::SetName(const ComPtr<ID3D12Resource> &object, const std::wstring &name)
{
    HRESULT hr = object->SetName(name.c_str());
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to set name for D3D12 object: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateSwapChain
(
    const ComPtr<IDXGIFactory4> &factory, 
    const ComPtr<ID3D12CommandQueue> &commandQueue, 
    const UINT &frameCount, UINT &frameIndex, 
    HWND hWnd, const UINT &clientWidth, const UINT &clientHeight, 
    ComPtr<IDXGISwapChain3> &swapChain
){
    HRESULT hr = E_FAIL;

    DXGI_SWAP_CHAIN_DESC1 desc = {};
    desc.BufferCount = frameCount;
    desc.Width = clientWidth;
    desc.Height = clientHeight;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    desc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain1;

    hr = factory->CreateSwapChainForHwnd
    (
        commandQueue.Get(),
        hWnd,
        &desc,
        nullptr,
        nullptr,
        &swapChain1
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create DXGI Swap Chain: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    hr = swapChain1.As(&(swapChain));
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to cast IDXGISwapChain1 to IDXGISwapChain3: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Get the current back buffer index
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

WBP_D3D12_API void wbp_d3d12::ResizeSwapChain
(
    const UINT &frameCount, UINT &frameIndex, 
    const UINT &clientWidth, const UINT &clientHeight, 
    const ComPtr<IDXGISwapChain3> &swapChain
){
    HRESULT hr = swapChain->ResizeBuffers
    (
        frameCount, clientWidth, clientHeight,
        DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to resize DXGI Swap Chain: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Get the current back buffer index
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

WBP_D3D12_API void wbp_d3d12::GetBuffersFromSwapChain
(
    const ComPtr<IDXGISwapChain3> &swapChain, const UINT &frameCount, 
    std::vector<ComPtr<ID3D12Resource>> &buffers
){
    HRESULT hr = E_FAIL;

    buffers.clear();
    for (UINT i = 0; i < frameCount; ++i)
    {
        ComPtr<ID3D12Resource> buffer;
        hr = swapChain->GetBuffer(i, IID_PPV_ARGS(buffer.GetAddressOf()));
        if (FAILED(hr))
        {
            std::string hrStr;
            wb::SpecifiedAs(hr, hrStr);

            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                { "Failed to get buffer from DXGI Swap Chain: ", hrStr }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
            wb::ThrowRuntimeError(err);
        }

        buffers.push_back(buffer);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateRenderTargetViewHeap
(
    const ComPtr<ID3D12Device4> &device, 
    const UINT &descriptorCount, ComPtr<ID3D12DescriptorHeap> &rtvHeap, UINT &rtvDescriptorSize
){
    HRESULT hr = E_FAIL;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = descriptorCount;
    heapDesc.NodeMask = 0;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    
    hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(rtvHeap.GetAddressOf()));
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Render Target View Heap: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Get the descriptor size for RTV
    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

WBP_D3D12_API void wbp_d3d12::CreateRenderTargetView
(
    const ComPtr<ID3D12Device4> &device, 
    const UINT &descriptorCount, std::vector<ComPtr<ID3D12Resource>> &renderTargets, 
    const ComPtr<ID3D12DescriptorHeap> &rtvHeap, UINT rtvDescriptorSize
){

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < descriptorCount; i++)
    {
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateCommandAllocator
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12CommandAllocator> &commandAllocator
){
    HRESULT hr = E_FAIL;
    
    hr = device->CreateCommandAllocator
    (
        D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Command Allocator: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateDepthStencil
(
    const ComPtr<ID3D12Device4> &device, 
    const UINT &clientWidth, const UINT &clientHeight, ComPtr<ID3D12Resource> &depthStencil
){
    HRESULT hr = E_FAIL;

    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    const CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
    const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D
    (
        DXGI_FORMAT_D24_UNORM_S8_UINT,
        clientWidth, clientHeight,
        1, 0, 1, 0,
        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
    );

    hr = device->CreateCommittedResource
    (
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &clearValue,
        IID_PPV_ARGS(depthStencil.GetAddressOf())
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Depth Stencil: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateDepthStencilViewHeap
(
    const ComPtr<ID3D12Device4> &device, 
    const UINT &depthStencilCount, ComPtr<ID3D12DescriptorHeap> &dsvHeap, UINT &dsvDescriptorSize
){
    HRESULT hr = E_FAIL;

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = depthStencilCount;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Depth Stencil View Heap: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Get the descriptor size for DSV
    dsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

WBP_D3D12_API void wbp_d3d12::CreateDepthStencilView
(
    const ComPtr<ID3D12Device4> &device,
    const UINT &descriptorCount, std::vector<ComPtr<ID3D12Resource>> &depthStencils,
    const ComPtr<ID3D12DescriptorHeap> &dsvHeap, UINT dsvDescriptorSize
){
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < descriptorCount; i++)
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        desc.Flags = D3D12_DSV_FLAG_NONE;

        device->CreateDepthStencilView(depthStencils[i].Get(), &desc, dsvHandle);
        dsvHandle.Offset(1, dsvDescriptorSize);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateViewport(D3D12_VIEWPORT &viewport, const UINT &clientWidth, const UINT &clientHeight)
{
    viewport.Width = static_cast<float>(clientWidth);
    viewport.Height = static_cast<float>(clientHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
}

WBP_D3D12_API void wbp_d3d12::CreateScissorRect(D3D12_RECT &scissorRect, const UINT &clientWidth, const UINT &clientHeight)
{
    scissorRect.top = 0;
    scissorRect.bottom = clientHeight;
    scissorRect.left = 0;
    scissorRect.right = clientWidth;
}

WBP_D3D12_API void wbp_d3d12::CreateBuffer
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12Resource> &buffer, const UINT &size
){
    HRESULT hr = E_FAIL;

    CD3DX12_HEAP_PROPERTIES upheapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

    UINT fixedSize = (size + 255) & ~255;
    CD3DX12_RESOURCE_DESC sourceDesc = CD3DX12_RESOURCE_DESC::Buffer(fixedSize);

    hr = device->CreateCommittedResource
    (
        &upheapProp,
        D3D12_HEAP_FLAG_NONE,
        &sourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(buffer.GetAddressOf())
    );

    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Buffer: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::UpdateBuffer
(
    const ComPtr<ID3D12Resource> &buffer, 
    const void *initData, const UINT &size
){
    HRESULT hr = E_FAIL;

    uint8_t* memory = nullptr;
    hr = buffer->Map(0, nullptr, reinterpret_cast<void**>(&memory));
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to map D3D12 Buffer: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
    }

    // Copy the initial data to the buffer
    std::memcpy(memory, initData, size);

    buffer->Unmap(0, nullptr);
}

WBP_D3D12_API void wbp_d3d12::CreateTexture2D
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12Resource> &texture, 
    const UINT &width, const UINT &height, const DXGI_FORMAT &format, const D3D12_RESOURCE_FLAGS &flags
){
    HRESULT hr = E_FAIL;

    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D
    (
        format, width, height, 1, 0, 1, 0,
        flags, D3D12_TEXTURE_LAYOUT_UNKNOWN
    );
    CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

    hr = device->CreateCommittedResource
    (
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(texture.GetAddressOf())
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Texture2D: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateUploadHeap
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12Resource> &uploadHeap, const UINT &size
){
    CD3DX12_HEAP_PROPERTIES upHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(size);

    device->CreateCommittedResource
    (
        &upHeap,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(uploadHeap.GetAddressOf())
    );
}

WBP_D3D12_API UINT wbp_d3d12::GetDXGIFormatByteSize(const DXGI_FORMAT &format)
{
    switch (format)
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return 4; // 32 bits = 4 bytes

    default:
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Unsupported DXGI format: ", std::to_string(static_cast<int>(format)) }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    return 0; // should not be reached
}

WBP_D3D12_API void wbp_d3d12::AddUploadTextureToCmdList
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12Resource> &texture, const ComPtr<ID3D12Resource> &uploadHeap, 
    const void *data, const UINT &width, const UINT &height, const DXGI_FORMAT &format
){
    // Create a subresource data structure to update the subresource data of a texture
    const UINT subresourceCount = 1;
    D3D12_SUBRESOURCE_DATA subresourceData = {};
    subresourceData.pData = data;
    subresourceData.RowPitch = (width * wbp_d3d12::GetDXGIFormatByteSize(format) + 255) & ~255; // Align to 256 bytes
    subresourceData.SlicePitch = subresourceData.RowPitch * height;

    // Added Texture Update Command to Command List
    // Transfer texture data to gpu using Upload heap. The data flow is pData->UploadHeap->Texture
    UpdateSubresources
    (
        cmdList.Get(), texture.Get(), uploadHeap.Get(), 0, 0, subresourceCount, &subresourceData
    );

    // Added resource barriers to update texture state
    CD3DX12_RESOURCE_BARRIER tra = CD3DX12_RESOURCE_BARRIER::Transition
    (
        texture.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
    );
    cmdList->ResourceBarrier(1, &tra);

    // Added an instruction to delete upload heap
    cmdList->DiscardResource(uploadHeap.Get(), nullptr );
}

WBP_D3D12_API void wbp_d3d12::CreateTexture2DSRV
(
    const ComPtr<ID3D12Device4> &device, 
    const ComPtr<ID3D12Resource> &resource, const DXGI_FORMAT &format,
    ComPtr<ID3D12DescriptorHeap> &srvHeap, const UINT &slotIndex
){
    UINT offset = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	size_t startAddr = srvHeap->GetCPUDescriptorHandleForHeapStart().ptr;

	D3D12_CPU_DESCRIPTOR_HANDLE heapHandle;

	// Describe and create a SRV for the texture.
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;

	heapHandle.ptr = startAddr + offset * slotIndex;

	device->CreateShaderResourceView(resource.Get(), &srvDesc, heapHandle);
}

WBP_D3D12_API void wbp_d3d12::CreateCommandList
(
    const ComPtr<ID3D12Device4> &device, 
    const ComPtr<ID3D12CommandAllocator> &commandAllocator, 
    ComPtr<ID3D12GraphicsCommandList> &commandList
){
    HRESULT hr = E_FAIL;

    hr = device->CreateCommandList
    (
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr,
        IID_PPV_ARGS(commandList.GetAddressOf())
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Command List: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    CloseCommand(commandList);
}

WBP_D3D12_API void wbp_d3d12::CreateFence
(
    const ComPtr<ID3D12Device4> &device, 
    ComPtr<ID3D12Fence> &fence, const UINT64 &initialValue
){
    HRESULT hr = E_FAIL;

    hr = device->CreateFence
    (
        initialValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())
    );
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create D3D12 Fence: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CreateFenceEvent(HANDLE &fenceEvent)
{
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to create fence event: "}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::CloseFenceEvent(HANDLE &fenceEvent)
{
    if (fenceEvent != nullptr)
    {
        bool result = CloseHandle(fenceEvent);
        if (!result)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                { "Failed to close fence event: " }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
            wb::ThrowRuntimeError(err);
        }

        fenceEvent = nullptr;
    }
}

WBP_D3D12_API void wbp_d3d12::CloseCommand(const ComPtr<ID3D12GraphicsCommandList> &commandList)
{
    HRESULT hr = commandList->Close();
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to close D3D12 Command List: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::ResetCommand
(
    const ComPtr<ID3D12GraphicsCommandList> &commandList, 
    const ComPtr<ID3D12CommandAllocator> &commandAllocator, 
    ID3D12PipelineState *pipelineState
){
    HRESULT hr = E_FAIL;

    hr = commandAllocator->Reset();
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to reset D3D12 Command Allocator: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    hr = commandList->Reset(commandAllocator.Get(), pipelineState);
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to reset D3D12 Command List: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

WBP_D3D12_API void wbp_d3d12::ExecuteCommand
(
    const ComPtr<ID3D12CommandQueue> &commandQueue, 
    const UINT &commandListCount, std::vector<ID3D12CommandList *> &commandLists
){
    commandQueue->ExecuteCommandLists
    (
        commandListCount, commandLists.data()
    );
}

WBP_D3D12_API void wbp_d3d12::WaitForGPU
(
    const ComPtr<ID3D12CommandQueue> &commandQueue, 
    const ComPtr<ID3D12Fence> &fence, UINT64 &fenceValue, HANDLE &fenceEvent
){
    HRESULT hr = E_FAIL;

    // Added fence detection signal to command queue
    hr = commandQueue->Signal(fence.Get(), fenceValue);
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to signal fence event", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err.c_str());
        wb::ThrowRuntimeError(err);
    }

    // Set a fence processing end confirmation event
    hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to set event on fence completion: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Waiting for event execution. No time limit
    WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);

    // Now that it's finished, the fence count is added for the corresponding frame.
    fenceValue++;
}

WBP_D3D12_API void wbp_d3d12::WaitFrameForGPU
(
    const ComPtr<ID3D12CommandQueue> &commandQueue, 
    const ComPtr<ID3D12Fence> &fence, HANDLE &fenceEvent, 
    const UINT64 &signalFenceValue, UINT64 &waitFenceValue
){
    HRESULT hr = E_FAIL;

    // Signal the command queue with the current fence value
    hr = commandQueue->Signal(fence.Get(), signalFenceValue);
    if (FAILED(hr))
    {
        std::string hrStr;
        wb::SpecifiedAs(hr, hrStr);

        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Failed to signal command queue: ", hrStr }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (fence->GetCompletedValue() < waitFenceValue)
    {
        // If the fence completion value is less than the value to wait, set the event.
        hr = fence->SetEventOnCompletion(waitFenceValue, fenceEvent);
        if (FAILED(hr))
        {
            std::string hrStr;
            wb::SpecifiedAs(hr, hrStr);

            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                { "Failed to set event on fence completion: ", hrStr }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
            wb::ThrowRuntimeError(err);
        }

        // Waiting for event execution. No time limit
        WaitForSingleObjectEx(fenceEvent, INFINITE, FALSE);
    }

    // Update fence completion value
    waitFenceValue = signalFenceValue + 1;
}

WBP_D3D12_API void wbp_d3d12::SetBarrier
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12Resource> &resource, 
    const D3D12_RESOURCE_STATES &beforeState, const D3D12_RESOURCE_STATES &afterState
){
    // Set a resource barrier to transition the resource state
    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition
    (
        resource.Get(), beforeState, afterState
    );
    
    cmdList->ResourceBarrier(1, &barrier);
}

WBP_D3D12_API void wbp_d3d12::SetRenderTargets
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12DescriptorHeap> &rtvHeap, const UINT &rtvIndex, const UINT &rtvDescriptorSize, 
    const ComPtr<ID3D12DescriptorHeap> &dsvHeap, const UINT &dsvIndex, const UINT &dsvDescriptorSize
){
    if (!rtvHeap)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Render Target View Heap is null." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (!dsvHeap)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Depth Stencil View Heap is null." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), rtvIndex, rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsvIndex, dsvDescriptorSize);
    cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

WBP_D3D12_API void wbp_d3d12::SetRenderTargets
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12DescriptorHeap> &rtvHeap, const UINT &rtvIndex, const UINT &rtvDescriptorSize
){
    if (!rtvHeap)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Render Target View Heap is null." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), rtvIndex, rtvDescriptorSize);
    cmdList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

WBP_D3D12_API void wbp_d3d12::ClearRenderTargetViews
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12DescriptorHeap> &rtvHeap, const UINT &rtvIndex, const UINT &rtvDescriptorSize, 
    const FLOAT (&clearColor)[4]
){
    if (!rtvHeap)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Render Target View Heap is null." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), rtvIndex, rtvDescriptorSize);
    cmdList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

WBP_D3D12_API void wbp_d3d12::ClearDepthStencilViews
(
    const ComPtr<ID3D12GraphicsCommandList> &cmdList, 
    const ComPtr<ID3D12DescriptorHeap> &dsvHeap, const UINT &dsvIndex, const UINT &dsvDescriptorSize, 
    const D3D12_RECT &scissorRect, const float &depth, const UINT8 &stencil
){
    if (!dsvHeap)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            { "Depth Stencil View Heap is null." }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart(), dsvIndex, dsvDescriptorSize);
    cmdList->ClearDepthStencilView
    (
        dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        depth, stencil, 1, &scissorRect
    );
}