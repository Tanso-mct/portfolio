#pragma once

#ifdef BDCCAGE_EXPORTS
#define BDC_CAGE_API __declspec(dllexport)
#else
#define BDC_CAGE_API __declspec(dllimport)
#endif