#pragma once
#include "wbp_d3d12/include/dll_config.h"

#include "windows_base/windows_base.h"

#include "wbp_d3d12/include/interfaces/window_d3d12_facade.h"
#include "wbp_d3d12/include/interfaces/swap_chain_context.h"
#include "wbp_d3d12/include/interfaces/render_target_context.h"
#include "wbp_d3d12/include/interfaces/fence_context.h"

namespace wbp_d3d12
{
    class WBP_D3D12_API WindowD3D12Facade : public wb::DefaultWindowFacade, public IWindowD3D12Facade
    {
    private:
        const UINT RENDER_TARGET_COUNT;
        const UINT DEPTH_STENCIL_COUNT;

        std::unique_ptr<ISwapChainContext> swapChainContext_ = nullptr;
        std::unique_ptr<IRenderTargetContext> renderTargetContext_ = nullptr;
        std::unique_ptr<IFenceContext> fenceContext_ = nullptr;

    public:
        WindowD3D12Facade(UINT renderTargetCount, UINT depthStencilCount);
        virtual ~WindowD3D12Facade() override = default;

        WindowD3D12Facade(const WindowD3D12Facade &) = delete;
        WindowD3D12Facade &operator=(const WindowD3D12Facade &) = delete;

        /***************************************************************************************************************
         * DefaultWindowFacade overrides
        /**************************************************************************************************************/

        virtual bool CheckIsReady() const override;

        virtual void Create(WNDCLASSEX& wc) override;
        virtual void Destroyed() override;
        virtual void Resized() override;

        /***************************************************************************************************************
         * IWindowD3D12Facade implementation
        /**************************************************************************************************************/

        virtual void SetSwapChainContext(std::unique_ptr<ISwapChainContext> swapChainContext) override;
        virtual void SetRenderTargetContext(std::unique_ptr<IRenderTargetContext> renderTargetContext) override;
        virtual void SetFenceContext(std::unique_ptr<IFenceContext> fenceContext) override;

        virtual void ResetCommand(ID3D12PipelineState *pipelineState) override;
        virtual void ResetCommand(ID3D12GraphicsCommandList *commandList, ID3D12PipelineState *pipelineState) override;
        virtual void CloseCommand() override;

        virtual void SetBarrierToRenderTarget() override;
        virtual void SetBarrierToRenderTarget(ID3D12GraphicsCommandList *commandList) override;
        virtual void SetBarrierToPresent() override;
        virtual void SetBarrierToPresent(ID3D12GraphicsCommandList *commandList) override;

        virtual void SetRenderTarget(UINT depthStencilIndex) override;
        virtual void SetRenderTarget(UINT depthStencilIndex, ID3D12GraphicsCommandList *commandList) override;

        virtual ID3D12CommandAllocator *GetCommandAllocator() override;
        virtual ID3D12GraphicsCommandList *GetCommandList() override;
        virtual const UINT &GetCurrentFrameIndex() const override;

        virtual void ClearViews(const float (&clearColor)[4], UINT depthStencilIndex) override;
        virtual void Present() override;

        virtual void WaitForGPU() override;
        virtual void WaitThisFrameForGPU() override;
    };
    
} // namespace wbp_d3d12