#pragma once
#include "windows_base/windows_base.h"
#include "wbp_d3d12/include/dll_config.h"

#include "wbp_d3d12/include/gpu_context.h"

namespace wbp_d3d12
{
    const WBP_D3D12_API size_t &GPUFacadeID();

    class WBP_D3D12_API GPUFacade : public wb::ISharedFacade
    {
    public:
        GPUFacade();
        ~GPUFacade() override = default;

        /***************************************************************************************************************
         * ISharedFacade implementation
        /**************************************************************************************************************/

        void SetContext(std::unique_ptr<wb::IContext> context) override;
        bool CheckIsReady() const override;
    };

} // namespace wbp_d3d12