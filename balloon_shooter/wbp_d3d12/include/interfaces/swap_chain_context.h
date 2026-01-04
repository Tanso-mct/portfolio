#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace wbp_d3d12
{
    class ISwapChainContext : public wb::IContext
    {
    public:
        virtual ~ISwapChainContext() = default;

        virtual Microsoft::WRL::ComPtr<IDXGISwapChain3> &GetSwapChain() = 0;
        virtual UINT &GetFrameCount() = 0;
        virtual UINT &GetFrameIndex() = 0;
        virtual UINT &GetSyncInterval() = 0;
    };

} // namespace wbp_d3d12