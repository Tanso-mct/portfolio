#pragma once

#ifdef MONOCYCLE_EXPORTS
#define MONO_CYCLE_API __declspec(dllexport)
#else
#define MONO_CYCLE_API __declspec(dllimport)
#endif