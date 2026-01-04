#pragma once

#ifdef WBPPRIMITIVE_EXPORTS
#define WBP_PRIMITIVE_API __declspec(dllexport)
#else
#define WBP_PRIMITIVE_API __declspec(dllimport)
#endif  