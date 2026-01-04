#pragma once
#include "wbp_d3d12/include/dll_config.h"

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgi1_4.h>
#include <wrl/client.h>

namespace wbp_d3d12
{
    class WBP_D3D12_API GPUContext : public wb::IContext
    {
    private:
        bool isCreated_ = false;
        Microsoft::WRL::ComPtr<IDXGIFactory4> factory_;
        Microsoft::WRL::ComPtr<ID3D12Device4> device_;
        Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_12_0; 

        // Singleton pattern
        GPUContext() = default;
        ~GPUContext() = default;

    public:
        GPUContext(const GPUContext&) = delete;
        GPUContext& operator=(const GPUContext&) = delete;

        static GPUContext& GetInstance();

        bool& IsCreated() { return isCreated_; }
        Microsoft::WRL::ComPtr<IDXGIFactory4>& GetFactory() { return factory_; }
        Microsoft::WRL::ComPtr<ID3D12Device4>& GetDevice() { return device_; }
        Microsoft::WRL::ComPtr<ID3D12CommandQueue>& GetCommandQueue() { return commandQueue_; }
        D3D_FEATURE_LEVEL& GetFeatureLevel() { return featureLevel; }
    };

} // namespace wbp_d3d12