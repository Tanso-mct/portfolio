#include "windows_base/src/pch.h"
#include "windows_base/include/console_log.h"

WINDOWS_BASE_API std::string wb::CreateMessage(const std::initializer_list<std::string_view> &messages)
{
    std::string msg = "";
    for (const auto &message : messages)
    {
        // Merge messages into a single string
        msg += message.data() + std::string("\n");
    }
    return msg;
}

WINDOWS_BASE_API std::string wb::CreateErrorMessage
(
    const std::string &file, int line, const std::string &function, 
    const std::initializer_list<std::string_view> &messages
){
    std::string errMsg = "";
    for (const auto &message : messages)
    {
        // Merge messages into a single error message
        errMsg += message.data() + std::string("\n");
    }

    errMsg += file + " : " + std::to_string(line) + "\n";
    errMsg += function + "\n";

    return errMsg;
}

WINDOWS_BASE_API void wb::ConsoleLog(std::string_view message)
{
#ifdef _DEBUG
    // Log to Windows Console
    std::cout << wb::CONSOLE_TEXT_COLOR_NORMAL;
    std::cout << message;
    std::cout << wb::CONSOLE_TEXT_COLOR_NORMAL;

    // Log to Visual Studio Debug Console
    OutputDebugStringA(message.data());

#endif
}

WINDOWS_BASE_API void wb::ConsoleLogWrn(std::string_view message)
{
#ifdef _DEBUG
    // Log to Windows Console
    std::cout << wb::CONSOLE_TEXT_COLOR_WARNING;
    std::cout << message;
    std::cout << wb::CONSOLE_TEXT_COLOR_NORMAL;

    // Log to Visual Studio Debug Console
    OutputDebugStringA(message.data());
#endif
}

WINDOWS_BASE_API void wb::ConsoleLogErr(std::string_view message)
{
    static bool alreadyLogged = false;

    // No need to log again if already logged
    if (alreadyLogged) return;

    // Set flag to prevent multiple logs
    alreadyLogged = true;

#ifdef _DEBUG
    // Log to Windows Console
    std::cout << wb::CONSOLE_TEXT_COLOR_ERROR;
    std::cout << message;
    std::cout << wb::CONSOLE_TEXT_COLOR_NORMAL;

    // Log to Visual Studio Debug Console
    OutputDebugStringA(message.data());
#endif
}