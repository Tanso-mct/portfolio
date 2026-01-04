#pragma once
#include "windows_base/windows_base.h"

#include <wrl/client.h>
#include <d3d12.h>

namespace wbp_texture
{
    class ITexture2DAsset : public wb::IAsset
    {
    public:
        virtual ~ITexture2DAsset() = default;

        virtual Microsoft::WRL::ComPtr<ID3D12Resource> &GetResource() = 0;
        virtual Microsoft::WRL::ComPtr<ID3D12Resource> &GetUploadHeap() = 0;

        virtual size_t &GetWidth() = 0;
        virtual size_t &GetHeight() = 0;
        virtual DXGI_FORMAT &GetFormat() = 0;
    };

} // namespace wbp_texture