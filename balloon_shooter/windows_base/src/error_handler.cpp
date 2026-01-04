#include "windows_base/src/pch.h"
#include "windows_base/include/error_handler.h"

WINDOWS_BASE_API void wb::ErrorNotify(std::string_view title, std::string_view message)
{
    static bool alreadyNotified = false;
    if (alreadyNotified)
    {
        // No need to notify again if already done
        return;
    }

    alreadyNotified = true; 

    MessageBoxA
    (
        nullptr,
        message.data(),
        title.data(),
        MB_OK | MB_ICONERROR | MB_TASKMODAL | MB_SETFOREGROUND | MB_TOPMOST
    );
}

WINDOWS_BASE_API void wb::ThrowRuntimeError(std::string_view message)
{
    throw std::runtime_error(message.data());
}