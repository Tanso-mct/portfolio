#pragma once

#ifdef mono_meta_extension_EXPORTS
#define MONO_META_EXT_DLL __declspec(dllexport)
#else
#define MONO_META_EXT_DLL __declspec(dllimport)
#endif