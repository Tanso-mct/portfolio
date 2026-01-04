#pragma once

#ifdef mono_forge_model_EXPORTS
#define MONO_FORGE_MODEL_DLL __declspec(dllexport)
#else
#define MONO_FORGE_MODEL_DLL __declspec(dllimport)
#endif