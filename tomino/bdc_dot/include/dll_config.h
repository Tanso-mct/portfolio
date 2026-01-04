#pragma once

#ifdef BDCDOT_EXPORTS
#define BDC_DOT_API __declspec(dllexport)
#else
#define BDC_DOT_API __declspec(dllimport)
#endif