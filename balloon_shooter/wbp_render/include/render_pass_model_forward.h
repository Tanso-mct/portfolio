#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/render_pass.h"

#include <wrl/client.h>

namespace wbp_render
{
    class ModelForwardRenderPass : public IRenderPass
    {
    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;

        std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> commandLists_;

        const UINT ROOT_SIGNATURE_VIEW_MATRIX = 2;
        const UINT ROOT_SIGNATURE_PROJECTION_MATRIX = 3;
        const UINT ROOT_SIGNATURE_WORLD_MATRIX = 0;
        const UINT ROOT_SIGNATURE_BONE_MATRIX = 1;
        const UINT ROOT_SIGNATURE_COLOR_CONFIG_BUFFER = 4;
        const UINT ROOT_SIGNATURE_TEXTURE_SRV = 5;

        Microsoft::WRL::ComPtr<ID3D12Resource> dummyTexture_ = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap_ = nullptr;
        size_t dummySRVSlot = 0;

        size_t currentSRVSlotSize_ = 0;
        std::unordered_map<size_t, size_t> textureSRVMap_; // First: texture asset ID, Second: descriptor heap index
        
    public:
        ModelForwardRenderPass() = default;
        ~ModelForwardRenderPass() override = default;

        ModelForwardRenderPass(const ModelForwardRenderPass &) = delete;
        ModelForwardRenderPass &operator=(const ModelForwardRenderPass &) = delete;

        void Initialize(ID3D12CommandAllocator *commandAllocator) override;

        ID3D12GraphicsCommandList* Execute
        (
            const size_t &currentFrameIndex,
            ID3D12Resource* cameraViewMatBuff, ID3D12Resource* cameraProjectionMatBuff,
            const wb::SystemArgument &args
        ) override;
    };

} // namespace wbp_render