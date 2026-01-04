#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/component_sprite_renderer.h"

#include <wrl/client.h>

namespace wbp_render
{
    const WBP_RENDER_API size_t &SpriteRendererComponentID();

    class WBP_RENDER_API SpriteRendererComponent : public ISpriteRendererComponent
    {
    private:
        bool textureAssetIDSet_ = false;
        size_t textureAssetID_;

        Microsoft::WRL::ComPtr<ID3D12Resource> clipMatBuffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> colorConfigBuffer_;

        SpriteColorConfig colorConfig_;

    public:
        SpriteRendererComponent();
        ~SpriteRendererComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * ISpriteRendererComponent implementation
        /**************************************************************************************************************/

        bool GetTextureAssetID(size_t &idDist) const override;
        void SetTextureAssetID(const size_t &texture2DAssetID);

        ID3D12Resource *GetClipMatBuffer() override { return clipMatBuffer_.Get(); }
        ID3D12Resource *GetColorConfigBuffer() override { return colorConfigBuffer_.Get(); }

        const SpriteColorConfig &GetColorConfig() const override { return colorConfig_; }
        void SetColorConfig(const SpriteColorConfig &config) override { colorConfig_ = config; }
    };

} // namespace wbp_render