#pragma once
#include "wbp_texture/include/dll_config.h"
#include "windows_base/windows_base.h"

#include <wrl/client.h>
#include <d3d12.h>

namespace wbp_texture
{
    const WBP_TEXTURE_API size_t &Texture2DAssetFactoryID();

    class WBP_TEXTURE_API Texture2DAssetFactory : public wb::IAssetFactory
    {
    private:
        static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
        static Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;

    public:
        Texture2DAssetFactory() = default;
        ~Texture2DAssetFactory() override = default;

        std::unique_ptr<wb::IAsset> Create(wb::IFileData &fileData) const override;
        void CreateAfter() override;
    };

} // namespace wbp_texture