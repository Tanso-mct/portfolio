#pragma once

#include <stdint.h>
#include <string_view>
#include <memory>
#include <cstdio>

#include "utility_header/logger.h"

namespace utility_header
{

// Load a file into memory
inline std::unique_ptr<uint8_t[]> LoadFile(std::string_view file_path, fpos_t &rt_data_size)
{
    FILE* fp = nullptr;
	errno_t error;

    // Open the file in binary read mode
	error = fopen_s(&fp, file_path.data(), "rb");
	if (error != 0)
    {
        utility_header::ConsoleLogErr({
            "File Load Error: Failed to open file. Path: ", file_path.data()
        }, __FILE__, __LINE__, __FUNCTION__);

		return nullptr;
    }

	// Move to the end of the file and calculate its size.
	fseek(fp, 0L, SEEK_END);
	fgetpos(fp, &rt_data_size);

	// Move to the beginning of the file
	fseek(fp, 0L, SEEK_SET);

    // Allocate buffer
	std::unique_ptr<uint8_t[]> buff = std::make_unique<uint8_t[]>(static_cast<size_t>(rt_data_size));

	// Read file data into buffer
	fread(buff.get(), 1, static_cast<size_t>(rt_data_size), fp);

    // Close the file
	fclose(fp);

    return buff;
}

// Get extension from file path
inline std::string_view GetFileExtension(std::string_view file_path)
{
	size_t dot_pos = file_path.rfind('.');
	if (dot_pos == std::string_view::npos)
		return ""; // No extension found

	return file_path.substr(dot_pos);
}

inline std::string GetFileNameFromPath(std::string_view file_path)
{
    size_t slash_pos = file_path.find_last_of("/\\");
    size_t dot_pos = file_path.rfind('.');

    size_t start = (slash_pos == std::string_view::npos) ? 0 : slash_pos + 1;
    size_t end = (dot_pos == std::string_view::npos || dot_pos < start) ? file_path.length() : dot_pos;

    return std::string(file_path.substr(start, end - start));
}

} // namespace utility_header