#pragma once

#ifdef WBPPHYSICS_EXPORTS
#define WBP_PHYSICS_API __declspec(dllexport)
#else
#define WBP_PHYSICS_API __declspec(dllimport)
#endif