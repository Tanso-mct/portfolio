#include "pch.h"

#include "include/mfm_converter.h"
#include "include/fbx_loader.h"
#include "include/file_utils.h"

TEST(MFM, Convert)
{
    // FBXファイルの読み込み
    model_converter::FBXLoader loader;
    std::unique_ptr<model_converter::IFileData> data = loader.Load("../resources/model_converter/cube.fbx");

    // MFM形式への変換
    model_converter::MFMConverter converter;
    u32 mfm_data_size = 0;
    std::unique_ptr<u8[]> mfm_data = converter.Convert(data.get(), mfm_data_size);
    EXPECT_NE(mfm_data, nullptr);
    EXPECT_GT(mfm_data_size, 0u);

    std::cout << "MFM data size: " << mfm_data_size << " bytes" << std::endl;

    // 変換後のデータをファイルに保存
    bool write_result = model_converter::WriteFile("../output/test_cube.mfm", mfm_data.get(), mfm_data_size);
    EXPECT_TRUE(write_result);
}