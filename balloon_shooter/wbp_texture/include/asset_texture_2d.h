#pragma once
#include "wbp_texture/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_texture/include/interfaces/asset_texture_2d.h"

namespace wbp_texture
{
    class Texture2DAsset : public ITexture2DAsset
    {
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
        Microsoft::WRL::ComPtr<ID3D12Resource> uploadHeap_;

        size_t width_;
        size_t height_;
        DXGI_FORMAT format_;

    public:
        Texture2DAsset() = default;
        ~Texture2DAsset() override = default;

        Microsoft::WRL::ComPtr<ID3D12Resource> &GetResource() override { return resource_; }
        Microsoft::WRL::ComPtr<ID3D12Resource> &GetUploadHeap() override { return uploadHeap_; }

        size_t &GetWidth() override { return width_; }
        size_t &GetHeight() override { return height_; }
        DXGI_FORMAT &GetFormat() override { return format_; }
    };

} // namespace wbp_texture