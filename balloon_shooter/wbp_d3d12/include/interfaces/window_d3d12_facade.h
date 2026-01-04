#pragma once

#include <d3d12.h>

#include "wbp_d3d12/include/interfaces/swap_chain_context.h"
#include "wbp_d3d12/include/interfaces/render_target_context.h"
#include "wbp_d3d12/include/interfaces/fence_context.h"

namespace wbp_d3d12
{
    class IWindowD3D12Facade
    {
    public:
        virtual ~IWindowD3D12Facade() = default;

        virtual void SetSwapChainContext(std::unique_ptr<ISwapChainContext> swapChainContext) = 0;
        virtual void SetRenderTargetContext(std::unique_ptr<IRenderTargetContext> renderTargetContext) = 0;
        virtual void SetFenceContext(std::unique_ptr<IFenceContext> fenceContext) = 0;

        virtual void ResetCommand(ID3D12PipelineState *pipelineState) = 0;
        virtual void ResetCommand(ID3D12GraphicsCommandList *commandList, ID3D12PipelineState *pipelineState) = 0;
        virtual void CloseCommand() = 0;

        virtual void SetBarrierToRenderTarget() = 0;
        virtual void SetBarrierToRenderTarget(ID3D12GraphicsCommandList *commandList) = 0;
        virtual void SetBarrierToPresent() = 0;
        virtual void SetBarrierToPresent(ID3D12GraphicsCommandList *commandList) = 0;

        virtual void SetRenderTarget(UINT depthStencilIndex) = 0;
        virtual void SetRenderTarget(UINT depthStencilIndex, ID3D12GraphicsCommandList *commandList) = 0;

        virtual ID3D12CommandAllocator *GetCommandAllocator() = 0;
        virtual ID3D12GraphicsCommandList *GetCommandList() = 0;
        virtual const UINT &GetCurrentFrameIndex() const = 0;

        virtual void ClearViews(const float (&clearColor)[4], UINT depthStencilIndex) = 0;
        virtual void Present() = 0;

        virtual void WaitForGPU() = 0;
        virtual void WaitThisFrameForGPU() = 0;

    };

} // namespace wbp_d3d12