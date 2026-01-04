#pragma once

#ifdef mono_asset_extension_EXPORTS
#define MONO_ASSET_EXT_DLL __declspec(dllexport)
#else
#define MONO_ASSET_EXT_DLL __declspec(dllimport)
#endif