#pragma once
#include "wbp_render/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_render/include/interfaces/component_mesh_renderer.h"

#include <wrl/client.h>

namespace wbp_render
{
    const WBP_RENDER_API size_t &MeshRendererComponentID();

    class WBP_RENDER_API MeshRenderComponent : public IMeshRendererComponent
    {
    private:
        size_t modelAssetID_;

        std::vector<bool> textureAssetIDsSet_;
        std::vector<size_t> textureAssetIDs_;

        Microsoft::WRL::ComPtr<ID3D12Resource> worldMatBuffer_;
        Microsoft::WRL::ComPtr<ID3D12Resource> colorConfigBuffer_;

        ModelColorConfig colorConfig_;

    public:
        MeshRenderComponent();
        ~MeshRenderComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * IMeshRendererComponent implementation
        /**************************************************************************************************************/

        const size_t &GetModelAssetID() const override { return modelAssetID_; }
        void SetModelAssetID(const size_t &modelAssetID) override { modelAssetID_ = modelAssetID; }

        bool GetTextureAssetID(TextureType textureType, size_t &idDist) const override;
        void AddTextureAssetID(const size_t &textureAssetID, TextureType textureType);

        ID3D12Resource *GetWorldMatBuffer() override { return worldMatBuffer_.Get(); }
        ID3D12Resource *GetColorConfigBuffer() override { return colorConfigBuffer_.Get(); }

        const ModelColorConfig &GetColorConfig() const override { return colorConfig_; }
        void SetColorConfig(const ModelColorConfig &config) override { colorConfig_ = config; }
    
    };

} // namespace wbp_render