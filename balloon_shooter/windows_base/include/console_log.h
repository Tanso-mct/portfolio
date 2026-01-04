#pragma once
#include "windows_base/include/dll_config.h"

#include <initializer_list>
#include <string_view>
#include <string>

namespace wb
{
    constexpr const char* CONSOLE_TEXT_COLOR_NORMAL = "\033[0m";
    constexpr const char* CONSOLE_TEXT_COLOR_WARNING = "\033[33m";
    constexpr const char* CONSOLE_TEXT_COLOR_ERROR = "\033[31m";

    WINDOWS_BASE_API std::string CreateMessage(const std::initializer_list<std::string_view> &messages);

    WINDOWS_BASE_API std::string CreateErrorMessage
    (
        const std::string& file, int line, const std::string& function,
        const std::initializer_list<std::string_view> &messages
    );

    WINDOWS_BASE_API void ConsoleLog(std::string_view message);
    WINDOWS_BASE_API void ConsoleLogWrn(std::string_view message);
    WINDOWS_BASE_API void ConsoleLogErr(std::string_view message);
    
} // namespace wb