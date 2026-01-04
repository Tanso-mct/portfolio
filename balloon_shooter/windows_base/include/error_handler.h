#pragma once
#include "windows_base/include/dll_config.h"

#include <string_view>

namespace wb
{
    WINDOWS_BASE_API void ErrorNotify(std::string_view title, std::string_view message);
    WINDOWS_BASE_API void ThrowRuntimeError(std::string_view message);
    
} // namespace wb