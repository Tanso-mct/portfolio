#pragma once

#ifdef WBPCOLLISION_EXPORTS
#define WBP_COLLISION_API __declspec(dllexport)
#else
#define WBP_COLLISION_API __declspec(dllimport)
#endif