#pragma once

#ifdef WBPRENDER_EXPORTS
#define WBP_RENDER_API __declspec(dllexport)
#else
#define WBP_RENDER_API __declspec(dllimport)
#endif