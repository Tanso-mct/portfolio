#include "include/pch.h"
#include "include/conversion_executor.h"

#include "include/interfaces/file_loader.h"
#include "include/interfaces/converter.h"
#include "include/file_utils.h"

namespace model_converter
{

ConversionExecutor::ConversionExecutor()
{
}

ConversionExecutor::~ConversionExecutor()
{
}

bool model_converter::ConversionExecutor::Setup(std::unique_ptr<IFileLoader> file_loader, std::unique_ptr<IConverter> converter)
{
    if (file_loader->GetSupportedFileExt() != converter->GetInputFileExt())
		return false;

	file_loader_ = std::move(file_loader);
	converter_ = std::move(converter);

	return true;
}

bool ConversionExecutor::Execute(std::string_view input_path, std::string_view output_path) const
{
	// ファイルの読み込み
	std::unique_ptr<IFileData> file_data = file_loader_->Load(input_path);
	if (!file_data)
	{
		std::cerr << "ファイルの読み込みに失敗しました。" << std::endl;
		return false;
	}

	// 変換処理
	u32 rt_data_size = 0;
	std::unique_ptr<u8[]> rt_data = converter_->Convert(file_data.get(), rt_data_size);
	if (!rt_data)
	{
		std::cerr << "ファイルの変換に失敗しました。" << std::endl;
		return false;
	}

	// ファイルの書き込み
	if (!WriteFile(output_path, rt_data.get(), rt_data_size))
	{
		std::cerr << "ファイルの書き込みに失敗しました。" << std::endl;
		return false;
	}

	return true;
}

ConversionPair::ConversionPair(std::string_view input_file_ext, std::string_view output_file_ext)
{
	input_file_ext_ = input_file_ext;
	output_file_ext_ = output_file_ext;
}

ConversionPair::~ConversionPair()
{
}

std::string_view ConversionPair::GetInputFileExt() const
{
	return input_file_ext_;
}

std::string_view ConversionPair::GetOutputFileExt() const
{
	return output_file_ext_;
}

bool ConversionPair::operator==(const ConversionPair& other) const
{
	return input_file_ext_ == other.GetInputFileExt() && output_file_ext_ == other.GetOutputFileExt();
}

} // namespace model_converter