#pragma once

#ifdef mono_transform_extension_EXPORTS
#define MONO_TRANSFORM_EXT_DLL __declspec(dllexport)
#else
#define MONO_TRANSFORM_EXT_DLL __declspec(dllimport)
#endif