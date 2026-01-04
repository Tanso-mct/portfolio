#pragma once
#include "windows_base/windows_base.h"

#include "directxtex/DirectXTex.h"

namespace wbp_png_loader
{
    class IPNGFileData : public wb::IFileData
    {
    public:
        virtual ~IPNGFileData() = default;

        virtual DirectX::ScratchImage &GetImage() = 0;
        virtual DirectX::TexMetadata &GetMetadata() = 0;
    };

} // namespace wbp_png_loader