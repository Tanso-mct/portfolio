#pragma once

#ifdef mono_graphics_extension_EXPORTS
#define MONO_GRAPHICS_EXT_DLL __declspec(dllexport)
#else
#define MONO_GRAPHICS_EXT_DLL __declspec(dllimport)
#endif