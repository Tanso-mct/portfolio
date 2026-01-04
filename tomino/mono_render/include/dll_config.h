#pragma once

#ifdef MONORENDER_EXPORTS
#define MONO_RENDER_API __declspec(dllexport)
#else
#define MONO_RENDER_API __declspec(dllimport)
#endif