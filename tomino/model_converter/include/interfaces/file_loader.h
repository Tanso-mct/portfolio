#pragma once

#include <string_view>
#include <memory>

namespace model_converter
{
    // 読み込んだデータのインターフェース
    class IFileData
    {
    public:
        virtual ~IFileData() = default;

        // このデータが対応するファイル形式の拡張子を取得
        virtual std::string_view GetFileExt() const = 0;
    };

    // ファイルを読み込むインターフェース
    class IFileLoader
    {
    public:
        virtual ~IFileLoader() = default;

        // このローダーが読み込めるファイル形式の拡張子を取得
        virtual std::string_view GetSupportedFileExt() const = 0;

        // ファイルの読み込み。成功したらIFileDataのインスタンスを返す。 失敗したらnullptrを返す。
        virtual std::unique_ptr<IFileData> Load(std::string_view file_path) const = 0;
    };

} // namespace model_converter