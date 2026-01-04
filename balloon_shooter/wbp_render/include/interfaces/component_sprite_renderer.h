#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>
#include <DirectXMath.h>

namespace wbp_render
{
    struct SpriteColorConfig
    {
        DirectX::XMFLOAT4 color;
        
        int isColorEnabled = true;
        int padding[3]; // Padding to align to 16 bytes
    };

    class ISpriteRendererComponent : public wb::IComponent
    {
    public:
        virtual ~ISpriteRendererComponent() override = default;

        virtual bool GetTextureAssetID(size_t &idDist) const = 0;
        virtual void SetTextureAssetID(const size_t &texture2DAssetID) = 0;

        virtual ID3D12Resource *GetClipMatBuffer() = 0;
        virtual ID3D12Resource *GetColorConfigBuffer() = 0;

        virtual const SpriteColorConfig &GetColorConfig() const = 0;
        virtual void SetColorConfig(const SpriteColorConfig &config) = 0;
    };

} // namespace wbp_render