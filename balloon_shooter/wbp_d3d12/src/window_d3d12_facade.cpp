#include "wbp_d3d12/src/pch.h"
#include "wbp_d3d12/include/window_d3d12_facade.h"

#include "wbp_d3d12/include/d3d12_helpers.h"
#include "wbp_d3d12/include/gpu_context.h"
#include "wbp_d3d12/include/swap_chain_context.h"
#include "wbp_d3d12/include/render_target_context.h"
#include "wbp_d3d12/include/fence_context.h"

#pragma comment(lib, "windows_base.lib")

wbp_d3d12::WindowD3D12Facade::WindowD3D12Facade(UINT renderTargetCount, UINT depthStencilCount) :
    RENDER_TARGET_COUNT(renderTargetCount),
    DEPTH_STENCIL_COUNT(depthStencilCount)
{
    if (renderTargetCount == 0)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Render target count must be greater than 0."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (depthStencilCount == 0)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Depth stencil count must be greater than 0."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }
}

bool wbp_d3d12::WindowD3D12Facade::CheckIsReady() const
{
    if (context_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Context is not set for this window facade."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (swapChainContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Swap chain context is not set for this window facade."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (renderTargetContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Render target context is not set for this window facade."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (fenceContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Fence context is not set for this window facade."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

void wbp_d3d12::WindowD3D12Facade::Create(WNDCLASSEX &wc)
{
    if (!CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window facade is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Create window
    /******************************************************************************************************************/

    wb::CreateWindowWB
    (
        context_->Handle(), context_->Style(), context_->Name().c_str(),
        context_->PosX(), context_->PosY(), context_->Width(), context_->Height(),
        context_->ParentHandle(), wc
    );

    // Keep the window's instance
    context_->Instance() = wc.hInstance;

    /*******************************************************************************************************************
     * Get the window's client area size
    /******************************************************************************************************************/

    {
        RECT clientRect;
        if (!GetClientRect(context_->Handle(), &clientRect))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to get client rectangle for the window."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->ClientWidth() = clientRect.right - clientRect.left;
        context_->ClientHeight() = clientRect.bottom - clientRect.top;

        POINT clientPos = {clientRect.left, clientRect.top};
        if (!ClientToScreen(context_->Handle(), &clientPos))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to convert client position to screen coordinates."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->PosX() = clientPos.x;
        context_->PosY() = clientPos.y;
    }

    /*******************************************************************************************************************
     * Enable raw input for the window
    /******************************************************************************************************************/

    {
        RAWINPUTDEVICE rawInputDevice[2];

        // Mouse
        rawInputDevice[0].usUsagePage = 0x01; // Generic Desktop Controls
        rawInputDevice[0].usUsage = 0x02; // Mouse
        rawInputDevice[0].dwFlags = 0;
        rawInputDevice[0].hwndTarget = context_->Handle();

        // Keyboard
        rawInputDevice[1].usUsagePage = 0x01; // Generic Desktop
        rawInputDevice[1].usUsage = 0x06; // Keyboard
        rawInputDevice[1].dwFlags = 0;
        rawInputDevice[1].hwndTarget = context_->Handle();

        if (!RegisterRawInputDevices(rawInputDevice, 2, sizeof(RAWINPUTDEVICE)))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to register raw input devices for the window."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }
    }

    /*******************************************************************************************************************
     * Check if the GPU context is created
    /******************************************************************************************************************/

    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"GPU context is not created. Cannot create window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Create swap chain
    /******************************************************************************************************************/

    swapChainContext_->GetFrameCount() = RENDER_TARGET_COUNT;

    wbp_d3d12::CreateSwapChain
    (
        gpuContext.GetFactory(), gpuContext.GetCommandQueue(),
        swapChainContext_->GetFrameCount(), swapChainContext_->GetFrameIndex(),
        context_->Handle(), context_->ClientWidth(), context_->ClientHeight(),
        swapChainContext_->GetSwapChain()
    );

    /*******************************************************************************************************************
     * Get render targets from the swap chain
    /******************************************************************************************************************/

    wbp_d3d12::GetBuffersFromSwapChain
    (
        swapChainContext_->GetSwapChain(), swapChainContext_->GetFrameCount(),
        renderTargetContext_->GetRenderTargets()
    );

    for (UINT i = 0; i < renderTargetContext_->GetRenderTargetCount(); i++)
    {
        wbp_d3d12::SetName
        (
            renderTargetContext_->GetRenderTargets()[i],
            L"SwapChain_RenderTarget" + std::to_wstring(i)
        );
    }

    /*******************************************************************************************************************
     * Create Render Target View
    /******************************************************************************************************************/

    wbp_d3d12::CreateRenderTargetViewHeap
    (
        gpuContext.GetDevice(), 
        renderTargetContext_->GetRenderTargetCount(),
        renderTargetContext_->GetRtvDescriptorHeap(), renderTargetContext_->GetRtvDescriptorSize()
    );

    wbp_d3d12::CreateRenderTargetView
    (
        gpuContext.GetDevice(), 
        renderTargetContext_->GetRenderTargetCount(), renderTargetContext_->GetRenderTargets(),
        renderTargetContext_->GetRtvDescriptorHeap(), renderTargetContext_->GetRtvDescriptorSize()
    );

    /*******************************************************************************************************************
     * Create Depth Stencil
    /******************************************************************************************************************/

    for (UINT i = 0; i < DEPTH_STENCIL_COUNT; i++)
    {
        wbp_d3d12::CreateDepthStencil
        (
            gpuContext.GetDevice(), 
            context_->ClientWidth(), context_->ClientHeight(),
            renderTargetContext_->GetDepthStencils()[i]
        );
    }

    for (UINT i = 0; i < DEPTH_STENCIL_COUNT; i++)
    {
        wbp_d3d12::SetName
        (
            renderTargetContext_->GetDepthStencils()[i],
            L"SwapChain_DepthStencil" + std::to_wstring(i)
        );
    }

    /*******************************************************************************************************************
     * Create Depth Stencil View
    /******************************************************************************************************************/

    wbp_d3d12::CreateDepthStencilViewHeap
    (
        gpuContext.GetDevice(), 
        renderTargetContext_->GetDepthStencilCount(),
        renderTargetContext_->GetDsvDescriptorHeap(), renderTargetContext_->GetDsvDescriptorSize()
    );

    wbp_d3d12::CreateDepthStencilView
    (
        gpuContext.GetDevice(), 
        renderTargetContext_->GetDepthStencilCount(), renderTargetContext_->GetDepthStencils(),
        renderTargetContext_->GetDsvDescriptorHeap(), renderTargetContext_->GetDsvDescriptorSize()
    );

    /*******************************************************************************************************************
     * Create the viewport and scissor rect
    /******************************************************************************************************************/

    wbp_d3d12::CreateViewport
    (
        renderTargetContext_->GetViewPort(),
        context_->ClientWidth(), context_->ClientHeight()
    );

    wbp_d3d12::CreateScissorRect
    (
        renderTargetContext_->GetScissorRect(),
        context_->ClientWidth(), context_->ClientHeight()
    );

    /*******************************************************************************************************************
     * Create command allocators and command lists
    /******************************************************************************************************************/

    for (UINT i = 0; i < RENDER_TARGET_COUNT; i++)
    {
        wbp_d3d12::CreateCommandAllocator
        (
            gpuContext.GetDevice(), renderTargetContext_->GetCommandAllocators()[i]
        );
    }

    for (UINT i = 0; i < RENDER_TARGET_COUNT; i++)
    {
        wbp_d3d12::CreateCommandList
        (
            gpuContext.GetDevice(), renderTargetContext_->GetCommandAllocators()[i],
            renderTargetContext_->GetCommandLists()[i]
        );
    }

    /*******************************************************************************************************************
     * Create fence
    /******************************************************************************************************************/

    wbp_d3d12::CreateFence
    (
        gpuContext.GetDevice(), fenceContext_->GetFence(), wbp_d3d12::INITIAL_FENCE_VALUE
    );

    wbp_d3d12::CreateFenceEvent(fenceContext_->GetFenceEvent());

    /*******************************************************************************************************************
     * Set the created flag
    /******************************************************************************************************************/

    context_->IsCreated() = true;
}

void wbp_d3d12::WindowD3D12Facade::Destroyed()
{
    /*******************************************************************************************************************
     * Show taskbar
    /******************************************************************************************************************/

    HWND taskbarWnd = FindWindow(wb::TASK_BAR_CLASS_NAME, nullptr);
    if (taskbarWnd)
    {
        ShowWindow(taskbarWnd, SW_SHOW);
    }
    else
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to find taskbar window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * GPU related cleanup
    /******************************************************************************************************************/

    // Wait for GPU to finish all commands
    WaitForGPU();

    // Release resource contexts
    renderTargetContext_.reset();
    swapChainContext_.reset();
    fenceContext_.reset();

    /*******************************************************************************************************************
     * Reset the created flag
    /******************************************************************************************************************/

    context_->IsCreated() = false;
}

void wbp_d3d12::WindowD3D12Facade::Resized()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Update the client area size
    /******************************************************************************************************************/

    {
        RECT clientRect;
        if (!GetClientRect(context_->Handle(), &clientRect))
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to get client rectangle for the window after resizing."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        context_->ClientWidth() = clientRect.right - clientRect.left;
        context_->ClientHeight() = clientRect.bottom - clientRect.top;

        // Ensure minimum size constraints
        if (context_->ClientWidth() < wb::MINIMUM_WINDOW_WIDTH)
        {
            context_->ClientWidth() = wb::MINIMUM_WINDOW_WIDTH;
        }
        if (context_->ClientHeight() < wb::MINIMUM_WINDOW_HEIGHT)
        {
            context_->ClientHeight() = wb::MINIMUM_WINDOW_HEIGHT;
        }
    }

    /*******************************************************************************************************************
     * Check if the GPU context is created
    /******************************************************************************************************************/

    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"GPU context is not created. Cannot resize window."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    /*******************************************************************************************************************
     * Release render targets and depth stencils
    /******************************************************************************************************************/

    for (UINT i = 0; i < renderTargetContext_->GetRenderTargetCount(); i++)
    {
        renderTargetContext_->GetRenderTargets()[i].Reset();
    }

    for (UINT i = 0; i < renderTargetContext_->GetDepthStencilCount(); i++)
    {
        renderTargetContext_->GetDepthStencils()[i].Reset();
    }

    /*******************************************************************************************************************
     * Resize swap chain
    /******************************************************************************************************************/

    wbp_d3d12::ResizeSwapChain
    (
        RENDER_TARGET_COUNT, swapChainContext_->GetFrameIndex(),
        context_->ClientWidth(), context_->ClientHeight(),
        swapChainContext_->GetSwapChain()
    );

    /*******************************************************************************************************************
     * Reset frame fence value
    /******************************************************************************************************************/

    for (UINT i = 0; i < RENDER_TARGET_COUNT; i++)
    {
        fenceContext_->GetFenceValues()[i] = wbp_d3d12::INITIAL_FENCE_VALUE;
    }

    /*******************************************************************************************************************
     * Get new render targets from the swap chain
    /******************************************************************************************************************/

    wbp_d3d12::GetBuffersFromSwapChain
    (
        swapChainContext_->GetSwapChain(), swapChainContext_->GetFrameCount(),
        renderTargetContext_->GetRenderTargets()
    );
    
    for (UINT i = 0; i < renderTargetContext_->GetRenderTargetCount(); i++)
    {
        wbp_d3d12::SetName
        (
            renderTargetContext_->GetRenderTargets()[i],
            L"SwapChain_RenderTarget" + std::to_wstring(i)
        );
    }

    /*******************************************************************************************************************
     * Create Render Target View
    /******************************************************************************************************************/

    wbp_d3d12::CreateRenderTargetView
    (
        gpuContext.GetDevice(),
        renderTargetContext_->GetRenderTargetCount(), renderTargetContext_->GetRenderTargets(),
        renderTargetContext_->GetRtvDescriptorHeap(), renderTargetContext_->GetRtvDescriptorSize()
    );

    /*******************************************************************************************************************
     * Create Depth Stencil
    /******************************************************************************************************************/

    for (UINT i = 0; i < renderTargetContext_->GetDepthStencilCount(); i++)
    {
        wbp_d3d12::CreateDepthStencil
        (
            gpuContext.GetDevice(),
            context_->ClientWidth(), context_->ClientHeight(),
            renderTargetContext_->GetDepthStencils()[i]
        );
    }

    /*******************************************************************************************************************
     * Create Depth Stencil View
    /******************************************************************************************************************/

    wbp_d3d12::CreateDepthStencilView
    (
        gpuContext.GetDevice(),
        renderTargetContext_->GetDepthStencilCount(), renderTargetContext_->GetDepthStencils(),
        renderTargetContext_->GetDsvDescriptorHeap(), renderTargetContext_->GetDsvDescriptorSize()
    );

    /*******************************************************************************************************************
     * Create the viewport and scissor rect
    /******************************************************************************************************************/

    wbp_d3d12::CreateViewport
    (
        renderTargetContext_->GetViewPort(),
        context_->ClientWidth(), context_->ClientHeight()
    );

    wbp_d3d12::CreateScissorRect
    (
        renderTargetContext_->GetScissorRect(),
        context_->ClientWidth(), context_->ClientHeight()
    );

    /*******************************************************************************************************************
     * Reset the resize flag
    /******************************************************************************************************************/

    context_->NeedsResize() = false;
}

void wbp_d3d12::WindowD3D12Facade::SetSwapChainContext(std::unique_ptr<ISwapChainContext> swapChainContext)
{
    if (swapChainContext == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Swap chain context cannot be null."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (swapChainContext_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Swap chain context is already set. Cannot set a new one."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    swapChainContext_ = std::move(swapChainContext);

    // Initialize swap chain context with render target and depth stencil counts
    swapChainContext_->GetFrameCount() = RENDER_TARGET_COUNT;
}

void wbp_d3d12::WindowD3D12Facade::SetRenderTargetContext(std::unique_ptr<IRenderTargetContext> renderTargetContext)
{
    if (renderTargetContext == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Render target context cannot be null."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (renderTargetContext_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Render target context is already set. Cannot set a new one."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    renderTargetContext_ = std::move(renderTargetContext);

    // Initialize render target context with render target and depth stencil counts
    renderTargetContext_->SetRenderTargetCount(RENDER_TARGET_COUNT);
    renderTargetContext_->SetDepthStencilCount(DEPTH_STENCIL_COUNT);
    renderTargetContext_->Resize();
}

void wbp_d3d12::WindowD3D12Facade::SetFenceContext(std::unique_ptr<IFenceContext> fenceContext)
{
    if (fenceContext == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Fence context cannot be null."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    if (fenceContext_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Fence context is already set. Cannot set a new one."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    fenceContext_ = std::move(fenceContext);

    // Initialize fence context with render target count
    fenceContext_->SetFenceCount(RENDER_TARGET_COUNT);
    fenceContext_->Resize();
}

void wbp_d3d12::WindowD3D12Facade::ResetCommand(ID3D12PipelineState *pipelineState)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::ResetCommand
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetCommandAllocators()[swapChainContext_->GetFrameIndex()],
        pipelineState
    );
}

void wbp_d3d12::WindowD3D12Facade::ResetCommand(ID3D12GraphicsCommandList *commandList, ID3D12PipelineState *pipelineState)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::ResetCommand
    (
        commandList,
        renderTargetContext_->GetCommandAllocators()[swapChainContext_->GetFrameIndex()],
        pipelineState
    );
}

void wbp_d3d12::WindowD3D12Facade::CloseCommand()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::CloseCommand(renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()]);
}

void wbp_d3d12::WindowD3D12Facade::SetBarrierToRenderTarget()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetBarrier
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetRenderTargets()[swapChainContext_->GetFrameIndex()],
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
}

void wbp_d3d12::WindowD3D12Facade::SetBarrierToRenderTarget(ID3D12GraphicsCommandList *commandList)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetBarrier
    (
        commandList,
        renderTargetContext_->GetRenderTargets()[swapChainContext_->GetFrameIndex()],
        D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET
    );
}

void wbp_d3d12::WindowD3D12Facade::SetBarrierToPresent()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetBarrier
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetRenderTargets()[swapChainContext_->GetFrameIndex()],
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
    );
}

void wbp_d3d12::WindowD3D12Facade::SetBarrierToPresent(ID3D12GraphicsCommandList *commandList)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetBarrier
    (
        commandList,
        renderTargetContext_->GetRenderTargets()[swapChainContext_->GetFrameIndex()],
        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT
    );
}

void wbp_d3d12::WindowD3D12Facade::SetRenderTarget(UINT depthStencilIndex)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetRenderTargets
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetRtvDescriptorHeap(),
        swapChainContext_->GetFrameIndex(), renderTargetContext_->GetRtvDescriptorSize(),
        renderTargetContext_->GetDsvDescriptorHeap(),
        depthStencilIndex, renderTargetContext_->GetDsvDescriptorSize()
    );

    renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()]->RSSetViewports
    (
        1, &renderTargetContext_->GetViewPort()
    );

    renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()]->RSSetScissorRects
    (
        1, &renderTargetContext_->GetScissorRect()
    );
}

void wbp_d3d12::WindowD3D12Facade::SetRenderTarget(UINT depthStencilIndex, ID3D12GraphicsCommandList *commandList)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::SetRenderTargets
    (
        commandList,
        renderTargetContext_->GetRtvDescriptorHeap(),
        swapChainContext_->GetFrameIndex(), renderTargetContext_->GetRtvDescriptorSize(),
        renderTargetContext_->GetDsvDescriptorHeap(),
        depthStencilIndex, renderTargetContext_->GetDsvDescriptorSize()
    );

    commandList->RSSetViewports(1, &renderTargetContext_->GetViewPort());
    commandList->RSSetScissorRects(1, &renderTargetContext_->GetScissorRect());
}

ID3D12CommandAllocator *wbp_d3d12::WindowD3D12Facade::GetCommandAllocator()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return renderTargetContext_->GetCommandAllocators()[swapChainContext_->GetFrameIndex()].Get();
}

ID3D12GraphicsCommandList *wbp_d3d12::WindowD3D12Facade::GetCommandList()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()].Get();
}

const UINT &wbp_d3d12::WindowD3D12Facade::GetCurrentFrameIndex() const
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return swapChainContext_->GetFrameIndex();
}

void wbp_d3d12::WindowD3D12Facade::ClearViews(const float (&clearColor)[4], UINT depthStencilIndex)
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::ClearRenderTargetViews
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetRtvDescriptorHeap(),
        swapChainContext_->GetFrameIndex(), renderTargetContext_->GetRtvDescriptorSize(),
        clearColor
    );

    wbp_d3d12::ClearDepthStencilViews
    (
        renderTargetContext_->GetCommandLists()[swapChainContext_->GetFrameIndex()],
        renderTargetContext_->GetDsvDescriptorHeap(),
        depthStencilIndex, renderTargetContext_->GetDsvDescriptorSize(),
        renderTargetContext_->GetScissorRect()
    );
}

void wbp_d3d12::WindowD3D12Facade::Present()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    swapChainContext_->GetSwapChain()->Present
    (
        swapChainContext_->GetSyncInterval(), 0
    );
}

void wbp_d3d12::WindowD3D12Facade::WaitForGPU()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Check if the gpu context is created
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"GPU context is not created. Cannot wait for GPU."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    wbp_d3d12::WaitForGPU
    (
        gpuContext.GetCommandQueue(), fenceContext_->GetFence(),
        fenceContext_->GetFenceValues()[swapChainContext_->GetFrameIndex()], fenceContext_->GetFenceEvent()
    );
}

void wbp_d3d12::WindowD3D12Facade::WaitThisFrameForGPU()
{
    if (!context_->IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Window is not created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Check if the gpu context is created
    wbp_d3d12::GPUContext &gpuContext = wbp_d3d12::GPUContext::GetInstance();
    if (!gpuContext.IsCreated())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"GPU context is not created. Cannot wait for GPU this frame."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE_PLUGIN_D3D12", err);
        wb::ThrowRuntimeError(err);
    }

    // Get current frame's fence value
    const UINT64 &fenceValue = fenceContext_->GetFenceValues()[swapChainContext_->GetFrameIndex()];

    // Update frame index
    swapChainContext_->GetFrameIndex() = swapChainContext_->GetSwapChain()->GetCurrentBackBufferIndex();

    wbp_d3d12::WaitFrameForGPU
    (
        gpuContext.GetCommandQueue(), fenceContext_->GetFence(), fenceContext_->GetFenceEvent(), 
        fenceValue, fenceContext_->GetFenceValues()[swapChainContext_->GetFrameIndex()]
    );
}