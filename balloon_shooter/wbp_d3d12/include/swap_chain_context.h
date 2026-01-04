#pragma once
#include "wbp_d3d12/include/dll_config.h"

#include "wbp_d3d12/include/interfaces/swap_chain_context.h"

namespace wbp_d3d12
{
    constexpr UINT DEFAULT_SYNC_INTERVAL = 1; // Default to 1 for VSync

    class SwapChainContext : public ISwapChainContext
    {
    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain_ = nullptr;
        UINT frameCount_ = 0;
        UINT frameIndex_ = 0;
        UINT syncInterval_ = DEFAULT_SYNC_INTERVAL;

    public:
        SwapChainContext() = default;

        Microsoft::WRL::ComPtr<IDXGISwapChain3>& GetSwapChain() override { return swapChain_; }
        UINT& GetFrameCount() override { return frameCount_; }
        UINT& GetFrameIndex() override { return frameIndex_; }
        UINT& GetSyncInterval() override { return syncInterval_; }
    };
} // namespace wbp_d3d12