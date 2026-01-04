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

} // namespace utility_header