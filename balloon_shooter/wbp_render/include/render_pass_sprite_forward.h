#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/render_pass.h"

#include "wbp_model/include/interfaces/asset_model.h"

#include <wrl/client.h>
#include <DirectXMath.h>

namespace wbp_render
{
    struct SpriteVertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    };

    class SpriteForwardRenderPass : public IRenderPass
    {
    private:
        Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
        Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
        Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
        const UINT ROOT_SIGNATURE_CLIP_MAT_BUFFER = 0;
        const UINT ROOT_SIGNATURE_COLOR_CONFIG_BUFFER = 1;
        const UINT ROOT_SIGNATURE_TEXTURE_SRV = 2;

        std::vector<Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>> commandLists_;
        wbp_model::MeshGPUData squareMeshData_;

        Microsoft::WRL::ComPtr<ID3D12Resource> dummyTexture_ = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap_ = nullptr;
        size_t dummySRVSlot = 0;

        size_t currentSRVSlotSize_ = 0;
        std::unordered_map<size_t, size_t> textureSRVMap_; // First: texture asset ID, Second: descriptor heap index

    public:
        SpriteForwardRenderPass() = default;
        ~SpriteForwardRenderPass() override = default;

        void Initialize(ID3D12CommandAllocator *commandAllocator) override;

        ID3D12GraphicsCommandList* Execute
        (
            const size_t &currentFrameIndex,
            ID3D12Resource* cameraViewMatBuff, ID3D12Resource* cameraProjectionMatBuff,
            const wb::SystemArgument &args
        ) override;
    };

} // namespace wbp_render