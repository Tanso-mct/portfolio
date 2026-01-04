#include "mono_file/src/pch.h"
#include "mono_file/include/png.h"

#pragma comment(lib, "riaecs.lib")

#pragma comment(lib, "DirectXTex.lib")
using namespace DirectX;

mono_file::FileDataPNG::FileDataPNG()
{
}

mono_file::FileDataPNG::~FileDataPNG()
{
}

mono_file::FileLoaderPNG::FileLoaderPNG()
{
}

mono_file::FileLoaderPNG::~FileLoaderPNG()
{
}

std::unique_ptr<riaecs::IFileData> mono_file::FileLoaderPNG::Load(std::string_view filePath) const
{
    std::unique_ptr<FileDataPNG> pngData = std::make_unique<FileDataPNG>();

    ScratchImage image;
    TexMetadata metadata;

    HRESULT hr = DirectX::LoadFromWICFile(
        std::wstring(filePath.begin(), filePath.end()).c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, &metadata, image);

    if (FAILED(hr))
        riaecs::NotifyError({"Failed to load PNG file.", "File path: " + std::string(filePath)}, RIAECS_LOG_LOC);

    if (metadata.format != DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        DirectX::ScratchImage converted;
        hr = DirectX::Convert
        (
            image.GetImages(), image.GetImageCount(), metadata,
            DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_DEFAULT, 0.0f, converted
        );
        
        if (SUCCEEDED(hr)) 
        {
            image = std::move(converted);
            metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;
        }
        else
        {
            riaecs::NotifyError({
                "Failed to convert PNG image to DXGI_FORMAT_R8G8B8A8_UNORM.",
                "File path: " + std::string(filePath)
            }, RIAECS_LOG_LOC);
        }
    }

    pngData->SetImage(std::move(image));
    pngData->SetMetadata(std::move(metadata));

    return pngData;
}

MONO_FILE_API riaecs::FileLoaderRegistrar<mono_file::FileLoaderPNG> mono_file::FileLoaderPNGID;