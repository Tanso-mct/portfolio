#pragma once
#include "mono_file/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "directxtex/DirectXTex.h"

namespace mono_file
{
    class MONO_FILE_API FileDataPNG : public riaecs::IFileData
    {
    private:
        DirectX::ScratchImage image_;
        DirectX::TexMetadata metadata_;

    public:
        FileDataPNG();
        ~FileDataPNG() override;

        const DirectX::ScratchImage& GetImage() const { return image_; }
        void SetImage(DirectX::ScratchImage &&image) { image_ = std::move(image); }

        const DirectX::TexMetadata& GetMetadata() const { return image_.GetMetadata(); }
        void SetMetadata(DirectX::TexMetadata &&metadata) { metadata_ = std::move(metadata); }
    };

    class MONO_FILE_API FileLoaderPNG : public riaecs::IFileLoader
    {
    public:
        FileLoaderPNG();
        ~FileLoaderPNG() override;

        std::unique_ptr<riaecs::IFileData> Load(std::string_view filePath) const override;
    };
    extern MONO_FILE_API riaecs::FileLoaderRegistrar<FileLoaderPNG> FileLoaderPNGID;

} // namespace mono_file