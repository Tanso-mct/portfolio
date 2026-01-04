#pragma once
#include "wbp_d3d12/include/dll_config.h"

#include "wbp_d3d12/include/interfaces/fence_context.h"

namespace wbp_d3d12
{
    constexpr UINT64 INITIAL_FENCE_VALUE = 0;

    class WBP_D3D12_API FenceContext : public wbp_d3d12::IFenceContext
    {
    private:
        UINT fenceCount_ = 0;
        Microsoft::WRL::ComPtr<ID3D12Fence> fence_ = nullptr;
        std::vector<UINT64> fenceValues_;
        HANDLE fenceEvent_ = nullptr;

    public:
        FenceContext() = default;
        ~FenceContext() override;

        /***************************************************************************************************************
         * IFenceContext implementation
        /**************************************************************************************************************/

        void SetFenceCount(UINT count) override { fenceCount_ = count; }
        void Resize() override;

        Microsoft::WRL::ComPtr<ID3D12Fence> &GetFence() override { return fence_; }
        std::vector<UINT64> &GetFenceValues() override { return fenceValues_; }
        HANDLE &GetFenceEvent() override { return fenceEvent_; }
    };

} // namespace wbp_d3d12