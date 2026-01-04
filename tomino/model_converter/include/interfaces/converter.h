#pragma once

#include "include/type.h"
#include <string_view>
#include <memory>

namespace model_converter
{
    class IFileData; // 読み込むデータのインターフェース

    // IFileDataを受け取りキャスト後、別形式に変換して保存するインターフェース
    class IConverter
    {
    public:
        virtual ~IConverter() = default;
        
        // 入力ファイル形式の拡張子を取得
        virtual std::string_view GetInputFileExt() const = 0;

        // 変換後のファイル形式の拡張子を取得
        virtual std::string_view GetConvertedFileExt() const = 0;

        // 変換処理
        virtual std::unique_ptr<u8[]> Convert(const IFileData* file_data, u32& rt_data_size) const = 0;
    };

} // namespace model_converter