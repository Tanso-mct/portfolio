#pragma once

#include <string>
#include <initializer_list>
#include <Windows.h>
#include <iostream>

namespace utility_header
{

constexpr const char* CONSOLE_TEXT_COLOR_DEFAULT = "\033[0m";
constexpr const char* CONSOLE_TEXT_COLOR_WARNING = "\033[33m";
constexpr const char* CONSOLE_TEXT_COLOR_ERROR = "\033[31m";

// Create a formatted message string from multiple lines and context information
inline std::string CreateMessage(const std::initializer_list<std::string> &lines)
{
    // Concatenate the message lines
    std::string log_message;
    for (const auto &line : lines)
        log_message += line + "\n";

    return log_message;
}

inline void AppendFileInfo(
    std::string &log_message, const std::string &file, int line, const std::string &function)
{
    log_message += "In " + file + ", line " + std::to_string(line) + ", in function " + function + "\n";
}

// Convert an HRESULT value to a human-readable string
inline std::string HrToString(HRESULT hr)
{
    char s_str[64] = {};
    sprintf_s(s_str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
    return std::string(s_str);
}

// Log an message to the console
inline void ConsoleLog(
    const std::initializer_list<std::string> &lines, 
    const std::string &file, int line, const std::string &function)
{
#ifdef _DEBUG

    // Create the message string
    std::string log_message = CreateMessage(lines);
    AppendFileInfo(log_message, file, line, function);

    // Output to the debug console
    OutputDebugStringA(log_message.c_str());

    // Also output to the standard console
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;
    std::cout << log_message;
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;

#endif // _DEBUG
}

inline void ConsoleLogWrn(
    const std::initializer_list<std::string> &lines, 
    const std::string &file, int line, const std::string &function)
{
#ifdef _DEBUG

    // Create the message string
    std::string log_message = CreateMessage(lines);
    AppendFileInfo(log_message, file, line, function);

    // Output to the debug console
    OutputDebugStringA(log_message.c_str());

    // Also output to the standard console with warning color
    std::cout << CONSOLE_TEXT_COLOR_WARNING;
    std::cout << log_message;
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;

#endif // _DEBUG
}

inline void ConsoleLogWrn(
    const std::initializer_list<std::string> &lines, HRESULT hr,
    const std::string &file, int line, const std::string &function)
{
#ifdef _DEBUG

    // Create the message string
    std::string log_message = CreateMessage(lines);
    log_message += "HRESULT: " + HrToString(hr) + "\n";
    AppendFileInfo(log_message, file, line, function);

    // Output to the debug console
    OutputDebugStringA(log_message.c_str());

    // Also output to the standard console with warning color
    std::cout << CONSOLE_TEXT_COLOR_WARNING;
    std::cout << log_message;
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;
#endif // _DEBUG
}

// Long an error message to the console
inline void ConsoleLogErr(
    const std::initializer_list<std::string> &lines, 
    const std::string &file, int line, const std::string &function)
{
    // Create the message string
    std::string log_message = CreateMessage(lines);
    AppendFileInfo(log_message, file, line, function);

    // Output to the debug console
    OutputDebugStringA(log_message.c_str());

    // Also output to the standard console with error color
    std::cout << CONSOLE_TEXT_COLOR_ERROR;
    std::cout << log_message;
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;
}

// Log an error log_message with an HRESULT value to the console
inline void ConsoleLogErr(
    const std::initializer_list<std::string> &lines, HRESULT hr,
    const std::string &file, int line, const std::string &function)
{
    // Create the message string
    std::string log_message = CreateMessage(lines);
    log_message += "HRESULT: " + HrToString(hr) + "\n";
    AppendFileInfo(log_message, file, line, function);

    // Output to the debug console
    OutputDebugStringA(log_message.c_str());

    // Also output to the standard console with error color
    std::cout << CONSOLE_TEXT_COLOR_ERROR;
    std::cout << log_message;
    std::cout << CONSOLE_TEXT_COLOR_DEFAULT;
}

} // namespace utility_header