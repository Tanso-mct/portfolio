#pragma once

#ifdef WBPFBXLOADER_EXPORTS
#define WBP_FBX_LOADER_API __declspec(dllexport)
#else
#define WBP_FBX_LOADER_API __declspec(dllimport)
#endif