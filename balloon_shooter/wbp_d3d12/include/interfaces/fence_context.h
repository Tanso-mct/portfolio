#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace wbp_d3d12
{
    class IFenceContext : public wb::IContext
    {
    public:
        virtual ~IFenceContext() = default;

        virtual void SetFenceCount(UINT count) = 0;
        virtual void Resize() = 0;

        virtual Microsoft::WRL::ComPtr<ID3D12Fence> &GetFence() = 0;
        virtual std::vector<UINT64> &GetFenceValues() = 0;
        virtual HANDLE &GetFenceEvent() = 0;
    };

} // namespace wbp_d3d12