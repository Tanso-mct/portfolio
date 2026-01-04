#pragma once

#ifdef MONOASSET_EXPORTS
#define MONO_ASSET_API __declspec(dllexport)
#else
#define MONO_ASSET_API __declspec(dllimport)
#endif