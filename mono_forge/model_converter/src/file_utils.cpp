#include "include/pch.h"
#include "include/file_utils.h"

namespace model_converter
{

std::string_view GetFileExt(std::string_view file_path)
{
    size_t dot_pos = file_path.rfind('.');
    if (dot_pos == std::string_view::npos)
        return ""; // 拡張子が見つからない場合は空文字を返す

    return file_path.substr(dot_pos, file_path.size() - dot_pos);
}

bool model_converter::WriteFile(std::string_view output_path, const u8 *data, u32 data_size)
{
    FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, output_path.data(), "wb");
    if (error != 0)
    {
        std::cerr << "ファイルの書き込みに失敗: " << output_path << std::endl;
        return false;
    }

	fwrite(data, sizeof(u8), data_size, fp);
	fclose(fp);

    return true;
}

} // namespace model_converter