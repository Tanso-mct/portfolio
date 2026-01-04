#include "mono_file_test/pch.h"

#include "riaecs/riaecs.h"
#pragma comment(lib, "riaecs.lib")

#include "mono_file/include/fbx.h"
#pragma comment(lib, "mono_file.lib")

TEST(FBX, Load)
{
    riaecs::ROObject<riaecs::IFileLoader> fileLoader = riaecs::gFileLoaderRegistry->Get(mono_file::FileLoaderFBXID());
    std::unique_ptr<riaecs::IFileData> fileData = fileLoader().Load("../resources/mono_file_test/fbx/box.fbx");
    ASSERT_NE(fileData, nullptr);

    mono_file::FileDataFBX &fbxData = static_cast<mono_file::FileDataFBX&>(*fileData);
    for (const auto &mesh : fbxData.GetMeshes())
    {
        std::cout << "Mesh Name: " << mesh.name_ << std::endl;
        std::cout << "Material Name: " << mesh.materialName_ << std::endl;
        std::cout << "Vertices Count: " << mesh.vertices_.size() << std::endl;
        std::cout << "Indices Count: " << mesh.indices_.size() << std::endl;
    }
}