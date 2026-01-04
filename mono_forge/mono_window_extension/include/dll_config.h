#pragma once

#ifdef mono_window_extension_EXPORTS
#define MONO_WINDOW_EXT_DLL __declspec(dllexport)
#else
#define MONO_WINDOW_EXT_DLL __declspec(dllimport)
#endif