#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace wbp_d3d12
{
    class IRenderTargetContext : public wb::IContext
    {
    public:
        virtual ~IRenderTargetContext() = default;

        virtual void SetRenderTargetCount(UINT count) = 0;
        virtual void SetDepthStencilCount(UINT count) = 0;
        virtual void Resize() = 0;

        virtual std::vector<Microsoft::WRL::ComPtr<ID3D12CommandAllocator>> &GetCommandAllocators() = 0;
        virtual std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> &GetCommandLists() = 0;

        virtual const UINT &GetRenderTargetCount() const = 0;
        virtual std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> &GetRenderTargets() = 0;
        virtual Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &GetRtvDescriptorHeap() = 0;
        virtual UINT &GetRtvDescriptorSize() = 0;

        virtual const UINT &GetDepthStencilCount() const = 0;
        virtual std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> &GetDepthStencils() = 0;
        virtual Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> &GetDsvDescriptorHeap() = 0;
        virtual UINT &GetDsvDescriptorSize() = 0;

        virtual D3D12_VIEWPORT& GetViewPort() = 0;
        virtual D3D12_RECT& GetScissorRect() = 0;
    };

} // namespace wbp_d3d12