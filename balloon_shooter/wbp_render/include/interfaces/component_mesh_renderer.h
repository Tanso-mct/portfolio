#pragma once
#include "windows_base/windows_base.h"

#include <d3d12.h>

namespace wbp_render
{
    enum class TextureType
    {
        Diffuse,
        Count
    };

    struct ModelColorConfig
    {
        int isColorEnabled = true;
        int padding[3];
    };

    class IMeshRendererComponent : public wb::IComponent
    {
    public:
        IMeshRendererComponent() = default;
        ~IMeshRendererComponent() override = default;

        virtual const size_t &GetModelAssetID() const = 0;
        virtual void SetModelAssetID(const size_t &modelAssetID) = 0;

        virtual bool GetTextureAssetID(TextureType textureType, size_t &idDist) const = 0;
        virtual void AddTextureAssetID(const size_t &textureAssetID, TextureType textureType) = 0;

        virtual ID3D12Resource *GetWorldMatBuffer() = 0;
        virtual ID3D12Resource *GetColorConfigBuffer() = 0;

        virtual const ModelColorConfig &GetColorConfig() const = 0;
        virtual void SetColorConfig(const ModelColorConfig &config) = 0;
    };

} // namespace wbp_render