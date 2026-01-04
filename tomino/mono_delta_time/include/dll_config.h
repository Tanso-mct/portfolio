#pragma once

#ifdef MONODELTATIME_EXPORTS
#define MONO_DELTA_TIME_API __declspec(dllexport)
#else
#define MONO_DELTA_TIME_API __declspec(dllimport)
#endif