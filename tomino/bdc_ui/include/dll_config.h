#pragma once

#ifdef BDCUI_EXPORTS
#define BDC_UI_API __declspec(dllexport)
#else
#define BDC_UI_API __declspec(dllimport)
#endif