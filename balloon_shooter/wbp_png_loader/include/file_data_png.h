#pragma once
#include "wbp_png_loader/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_png_loader/include/interfaces/file_data_png.h"

namespace wbp_png_loader
{
    class FileDataPNG : public IPNGFileData
    {
    private:
        DirectX::ScratchImage image_;
        DirectX::TexMetadata metadata_;

    public:
        FileDataPNG() = default;
        virtual ~FileDataPNG() = default;

        DirectX::ScratchImage &GetImage() override { return image_; }
        DirectX::TexMetadata &GetMetadata() override { return metadata_; }
    };

} // namespace wbp_png_loader