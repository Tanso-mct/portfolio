#pragma once

#ifdef MONOSOUND_EXPORTS
#define MONO_SOUND_API __declspec(dllexport)
#else
#define MONO_SOUND_API __declspec(dllimport)
#endif