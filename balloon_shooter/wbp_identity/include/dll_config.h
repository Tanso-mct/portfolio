#pragma once

#ifdef WBPIDENTITY_EXPORTS
#define WBP_IDENTITY_API __declspec(dllexport)
#else
#define WBP_IDENTITY_API __declspec(dllimport)
#endif