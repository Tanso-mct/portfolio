#pragma once

#ifdef MONOSCENE_EXPORTS
#define MONO_SCENE_API __declspec(dllexport)
#else
#define MONO_SCENE_API __declspec(dllimport)
#endif