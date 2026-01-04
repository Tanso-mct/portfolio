#include "mono_file_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_file/include/png.h"
#pragma comment(lib, "mono_file.lib")

TEST(PNG, Load)
{
    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    ASSERT_TRUE(SUCCEEDED(hr));

    riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderPNGID());
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load("../resources/mono_file_test/png/Lenna.png");
    ASSERT_NE(fileData, nullptr);

    mono_file::FileDataPNG &pngData = static_cast<mono_file::FileDataPNG&>(*fileData);
    const DirectX::TexMetadata &metadata = pngData.GetMetadata();
    std::cout << "Width: " << metadata.width << std::endl;
    std::cout << "Height: " << metadata.height << std::endl;
    std::cout << "Format: " << metadata.format << std::endl;

    CoUninitialize();
}