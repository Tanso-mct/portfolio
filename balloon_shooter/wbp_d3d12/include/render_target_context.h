#pragma once
#include "wbp_d3d12/include/dll_config.h"

#include "wbp_d3d12/include/interfaces/render_target_context.h"

namespace wbp_d3d12
{
    class WBP_D3D12_API RenderTargetContext : public wbp_d3d12::IRenderTargetContext
    {
    private:
        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> commandAllocators_;
        std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> commandLists_;

        UINT renderTargetCount_ = 0;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> renderTargets_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap_ = nullptr;
        UINT rtvDescriptorSize_ = 0;

        UINT depthStencilCount_ = 0;
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> depthStencils_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap_ = nullptr;
        UINT dsvDescriptorSize_ = 0;

        D3D12_VIEWPORT viewPort_;
        D3D12_RECT scissorRect_;

    public:
        RenderTargetContext() = default;
        ~RenderTargetContext() override = default;

        /***************************************************************************************************************
         * IRenderTargetContext implementation
        /**************************************************************************************************************/

        void SetRenderTargetCount(UINT count) override { renderTargetCount_ = count; }
        void SetDepthStencilCount(UINT count) override { depthStencilCount_ = count; }
        void Resize() override;

        std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> &GetCommandAllocators() override { return commandAllocators_; }
        std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> &GetCommandLists() override { return commandLists_; }

        const UINT &GetRenderTargetCount() const override { return renderTargetCount_; }
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> &GetRenderTargets() override { return renderTargets_; }
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &GetRtvDescriptorHeap() override { return rtvDescriptorHeap_; }
        UINT &GetRtvDescriptorSize() override { return rtvDescriptorSize_; }

        const UINT &GetDepthStencilCount() const override { return depthStencilCount_; }
        std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> &GetDepthStencils() override { return depthStencils_; }
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &GetDsvDescriptorHeap() override { return dsvDescriptorHeap_; }
        UINT &GetDsvDescriptorSize() override { return dsvDescriptorSize_; }

        D3D12_VIEWPORT& GetViewPort() override { return viewPort_; }
        D3D12_RECT& GetScissorRect() override { return scissorRect_; }
    };

} // namespace wbp_d3d12