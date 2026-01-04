#pragma once

#ifdef mono_scene_extension_EXPORTS
#define MONO_SCENE_EXT_DLL __declspec(dllexport)
#else
#define MONO_SCENE_EXT_DLL __declspec(dllimport)
#endif