#pragma once

#include <string_view>

#include "include/type.h"

namespace model_converter
{
    // ファイルパスから拡張子を取得する関数
    std::string_view GetFileExt(std::string_view file_path);

    // バイナリデータをファイルに書き込む関数
    bool WriteFile(std::string_view output_path, const u8* data, u32 data_size);

} // namespace model_converter