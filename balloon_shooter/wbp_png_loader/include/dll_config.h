#pragma once

#ifdef WBPPNGLOADER_EXPORTS
#define WBP_PNG_LOADER_API __declspec(dllexport)
#else
#define WBP_PNG_LOADER_API __declspec(dllimport)
#endif