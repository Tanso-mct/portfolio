#pragma once

#ifdef WBPLOCATOR_EXPORTS
#define WBP_LOCATOR_API __declspec(dllexport)
#else
#define WBP_LOCATOR_API __declspec(dllimport)
#endif