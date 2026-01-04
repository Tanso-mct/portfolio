#pragma once

#ifdef WBPTRANSFORM_EXPORTS
#define WBP_TRANSFORM_API __declspec(dllexport)
#else
#define WBP_TRANSFORM_API __declspec(dllimport)
#endif