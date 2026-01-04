#pragma once

#ifdef WINDOWSBASE_EXPORTS
#define WINDOWS_BASE_API __declspec(dllexport)
#else
#define WINDOWS_BASE_API __declspec(dllimport)
#endif