#include "pch.h"

#include "include/file_utils.h"

TEST(FileUtils, GetFileExt)
{
    // 拡張子あり
    {
        std::string_view file_path = "C:/path/to/file.fbx";
        std::string_view ext = model_converter::GetFileExt(file_path);
        EXPECT_EQ(ext, ".fbx");
    }

    // 拡張子なし
    {
        std::string_view file_path = "C:/path/to/file";
        std::string_view ext = model_converter::GetFileExt(file_path);
        EXPECT_EQ(ext, "");
    }

    // 複数ドット
    {
        std::string_view file_path = "C:/path.to/file.name.ext";
        std::string_view ext = model_converter::GetFileExt(file_path);
        EXPECT_EQ(ext, ".ext");
    }

    // ドットで終わる
    {
        std::string_view file_path = "C:/path/to/file.";
        std::string_view ext = model_converter::GetFileExt(file_path);
        EXPECT_EQ(ext, ".");
    }

    // 空文字列
    {
        std::string_view file_path = "";
        std::string_view ext = model_converter::GetFileExt(file_path);
        EXPECT_EQ(ext, "");
    }
}