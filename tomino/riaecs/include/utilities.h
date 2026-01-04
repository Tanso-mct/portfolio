#pragma once
#include "riaecs/include/dll_config.h"

#include <string>
#include <initializer_list>
#include <future>

#define RIAECS_LOG_LOC __FILE__, __LINE__, __FUNCTION__

namespace riaecs
{
    RIAECS_API std::string CreateMessage(const std::initializer_list<std::string> &lines);
    RIAECS_API std::string CreateMessage
    (
        const std::initializer_list<std::string> &lines, 
        const std::string &file, int line, const std::string &function
    );

    RIAECS_API void NotifyError
    (
        const std::initializer_list<std::string> &lines, 
        const std::string &file, int line, const std::string &function
    );

    RIAECS_API void CreateStandardConsole
    (
        const std::wstring &consoleName = L"Console", short fontSize = 16
    );

    RIAECS_API bool CheckFutureIsReady(std::future<void> &future);

} // namespace riaecs