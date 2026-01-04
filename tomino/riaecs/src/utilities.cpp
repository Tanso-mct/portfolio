#include "riaecs/src/pch.h"
#include "riaecs/include/utilities.h"

#include "riaecs/include/log.h"

RIAECS_API std::string riaecs::CreateMessage(const std::initializer_list<std::string> &lines)
{
    std::string message;
    for (const auto &line : lines)
        message += line + "\n";

    return message;
}

RIAECS_API std::string riaecs::CreateMessage
(
    const std::initializer_list<std::string> &lines, 
    const std::string &file, int line, const std::string &function
){
    std::string message = CreateMessage(lines);
    message += file + " : " + std::to_string(line) + " : " + function + "\n";

    return message;
}

RIAECS_API void riaecs::NotifyError
(
    const std::initializer_list<std::string> &lines, 
    const std::string &file, int line, const std::string &function)
{
    // Create the error message
    std::string log = riaecs::CreateMessage(lines, file, line, function);

    // Output the error message to console and error window
    riaecs::Log::OutToConsole(log, riaecs::CONSOLE_TEXT_COLOR_ERROR);
    riaecs::Log::OutToErrorWindow(log, "RIAECS");

    // Throw a runtime error with the log message
    throw std::runtime_error(log);
}

RIAECS_API void riaecs::CreateStandardConsole
(
    const std::wstring &consoleName, short fontSize
){
    // If the console window already exists, do nothing
    if (GetConsoleWindow() != NULL) return;

    // Allocate a new console for the application
    AllocConsole();
    freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
    freopen_s(reinterpret_cast<FILE**>(stderr), "CONOUT$", "w", stderr);

    // Sync C++ streams with the console
    std::ios::sync_with_stdio(true);
    std::wcout.clear();
    std::cout.clear();
    std::wcerr.clear();
    std::cerr.clear();
    std::wclog.clear();
    std::clog.clear();

    // Get the standard output handle
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdout == INVALID_HANDLE_VALUE)
        riaecs::NotifyError({"Failed to get standard output handle."}, RIAECS_LOG_LOC);

    // Get the current console mode
    DWORD mode = 0;
    if (!GetConsoleMode(hStdout, &mode))
        riaecs::NotifyError({"Failed to get console mode."}, RIAECS_LOG_LOC);

    // Enable ANSI sequence support
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hStdout, mode))
        riaecs::NotifyError({"Failed to set console mode."}, RIAECS_LOG_LOC);

    // Set the console title
    if (!SetConsoleTitleW(consoleName.c_str()))
        riaecs::NotifyError({"Failed to set console title."}, RIAECS_LOG_LOC);

    // Set the console text font
    CONSOLE_FONT_INFOEX cfi = {0};
    cfi.cbSize = sizeof(cfi);
    cfi.nFont = 0;
    cfi.dwFontSize.X = 0;
    cfi.dwFontSize.Y = fontSize;
    cfi.FontFamily = FF_DONTCARE;
    cfi.FontWeight = FW_NORMAL;
    if (!SetCurrentConsoleFontEx(hStdout, FALSE, &cfi))
        riaecs::NotifyError({"Failed to set console font."}, RIAECS_LOG_LOC);
}

RIAECS_API bool riaecs::CheckFutureIsReady(std::future<void> &future)
{
    if (!future.valid())
        return true;

    return future.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
}