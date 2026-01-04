#pragma once

#include <memory>
#include <string_view>

namespace model_converter
{
    class IFileLoader; // ファイルを読み込むインターフェース
    class IConverter; // 変換を行うインターフェース

    // LoaderとConverterを組み合わせて変換を実行するクラス
    // 作成時にLoaderとConverterが適した組み合わせでない場合、例外を投げる
    class ConversionExecutor
    {
    public:
        ConversionExecutor();
        ~ConversionExecutor();
        
        bool Setup(std::unique_ptr<IFileLoader> file_loader, std::unique_ptr<IConverter> converter);
        bool Execute(std::string_view input_path, std::string_view output_path) const;

    private:
        std::unique_ptr<IFileLoader> file_loader_;
        std::unique_ptr<IConverter> converter_;
    };

    // LoaderとConverterの組み合わせを表すクラス
    // unordered_mapのキーとして使用するため、==演算子をオーバーロードしている
    class ConversionPair
    {
    public:
        ConversionPair(std::string_view input_file_ext, std::string_view output_file_ext);
        ~ConversionPair();

        std::string_view GetInputFileExt() const;
        std::string_view GetOutputFileExt() const;

        bool operator==(const ConversionPair& other) const;

    private:
        std::string input_file_ext_;
        std::string output_file_ext_;
    };

} // namespace model_converter

// ConversionPairをunordered_mapのキーとして使用するためのstd::hashの特殊化
namespace std 
{
    template<>
    struct hash<model_converter::ConversionPair> 
    {
        std::size_t operator()(const model_converter::ConversionPair& pair) const {
            return 
                std::hash<std::string_view>()(pair.GetInputFileExt()) ^ 
                (std::hash<std::string_view>()(pair.GetOutputFileExt()) << 1);
        }
    };
}