#include "include/pch.h"

#include "include/file_utils.h"
#include "include/conversion_executor.h"

#include "include/fbx_loader.h"
#include "include/mfm_converter.h"

int main(int argc, char* argv[])
{
    /*******************************************************************************************************************
     * 変換ペアとそれに対応するExecutorのマップを作成
    /******************************************************************************************************************/

    std::unordered_map<model_converter::ConversionPair, std::unique_ptr<model_converter::ConversionExecutor>> executors;
    using ExecutorElement = std::pair<const model_converter::ConversionPair, std::unique_ptr<model_converter::ConversionExecutor>>;

    // FBX -> MFM
    {
        std::unique_ptr<model_converter::IFileLoader> loader = std::make_unique<model_converter::FBXLoader>();
        std::unique_ptr<model_converter::IConverter> converter = std::make_unique<model_converter::MFMConverter>();
        model_converter::ConversionPair pair(loader->GetSupportedFileExt(), converter->GetConvertedFileExt());
        std::unique_ptr<model_converter::ConversionExecutor> executor = std::make_unique<model_converter::ConversionExecutor>();
        if (!executor->Setup(std::move(loader), std::move(converter)))
        {
            std::cerr << "ConversionExecutorのセットアップに失敗しました。" << std::endl;
            return -1;
        }

        // Executorの登録
        executors.emplace(std::move(pair), std::move(executor));
    }

    /*******************************************************************************************************************
     * コマンドライン引数の解析
    /******************************************************************************************************************/

    // 引数の数が正しいか確認
    if (argc != 5)
    {
        std::cout << "引数の数が合いません。以下の例のように実行してください。" << std::endl;
        std::cout << "model_converter.exe /i 入力ファイルパス /o 出力ファイルパス" << std::endl;
        return -1;
    }

    // /i、/oがそれぞれ一つずつ指定されているか確認
    int argument_collect = 1;
    argument_collect *= (std::string(argv[1]) == "/i" && std::string(argv[3]) == "/i") ? 0 : 1;
    argument_collect *= (std::string(argv[1]) == "/o" && std::string(argv[3]) == "/o") ? 0 : 1;
    if (argument_collect != 1)
    {
        std::cout << "引数が不正です。" << std::endl;
        std::cout << "/i、/oを使用し、入力ファイルパス、出力ファイルパスを指定してください。" << std::endl;
        return -1;
    }

    std::string input_path;
    std::string output_path;
    for (int i = 1; i <= 3; i += 2)
    {
        if (std::string(argv[i]) == "/i") input_path = argv[i+1]; // 入力ファイルパスを取得
        else if (std::string(argv[i]) == "/o") output_path = argv[i+1]; // 出力ファイルパスパスを取得
    }

    /*******************************************************************************************************************
     * 変換ペアの確認と、変換の実行
    /******************************************************************************************************************/
    
    // 変換元と、変換先の拡張子から変換ペアを作成
    model_converter::ConversionPair target_pair(
        model_converter::GetFileExt(input_path), model_converter::GetFileExt(output_path));

    // 変換ペアが登録されているか確認
    if (executors.find(target_pair) == executors.end())
    {
        std::cout << "指定された変換ペアは登録されていません。" << std::endl;
        return -1;
    }

    // 変換ペアに対応するExecutorを取得して実行
    const model_converter::ConversionExecutor& executor = *executors[target_pair];
    bool result = executor.Execute(input_path, output_path);

    if (result)
    {
        std::cout << "変換に成功しました。" << std::endl;
        return 0;
    }
    else
    {
        std::cout << "変換に失敗しました。" << std::endl;
        return -1;
    }
}