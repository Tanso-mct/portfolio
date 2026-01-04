#pragma once

#ifdef WBPTEXTURE_EXPORTS
#define WBP_TEXTURE_API __declspec(dllexport)
#else
#define WBP_TEXTURE_API __declspec(dllimport)
#endif