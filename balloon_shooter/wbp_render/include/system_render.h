#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/render_pass.h"

#include <wrl/client.h>

namespace wbp_render
{
    const WBP_RENDER_API size_t &RenderSystemID();

    constexpr UINT RENDER_TARGET_COUNT = 2;
    constexpr UINT DEPTH_STENCIL_COUNT = 1;
    constexpr UINT DEPTH_STENCIL_FOR_DRAW = 0;

    // rgb(137, 195, 221)

    const float CLEAR_COLOR[4] = { 137.0f / 256.0f,  195.0f / 256.0f, 221.0f / 256.0f, 1.0f };

    class WBP_RENDER_API RenderSystem : public wb::ISystem
    {
    private:
        bool passesInitialized_ = false;
        std::vector<std::unique_ptr<IRenderPass>> passes_;

        Microsoft::WRL::ComPtr<ID3D12Resource> cameraViewMatBuff_;
        Microsoft::WRL::ComPtr<ID3D12Resource> cameraProjectionMatBuff_;

    public:
        RenderSystem() = default;
        ~RenderSystem() override;

        RenderSystem(const RenderSystem &) = delete;
        RenderSystem &operator=(const RenderSystem &) = delete;

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;

    };

} // namespace wbp_render