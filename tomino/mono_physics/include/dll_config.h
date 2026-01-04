#pragma once

#ifdef MONOPHYSICS_EXPORTS
#define MONO_PHYSICS_API __declspec(dllexport)
#else
#define MONO_PHYSICS_API __declspec(dllimport)
#endif