#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>

namespace wbp_render
{
    class IRenderPass
    {
    public:
        IRenderPass() = default;
        virtual ~IRenderPass() = default;

        virtual void Initialize(ID3D12CommandAllocator *commandAllocator) = 0;

        virtual ID3D12GraphicsCommandList* Execute
        (
            const size_t &currentFrameIndex,
            ID3D12Resource* cameraViewMatBuff, ID3D12Resource* cameraProjectionMatBuff,
            const wb::SystemArgument &args
        ) = 0;
    };

} // namespace wbp_render