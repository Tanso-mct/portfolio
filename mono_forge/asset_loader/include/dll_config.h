#pragma once

#ifdef asset_loader_EXPORTS
#define ASSET_LOADER_DLL __declspec(dllexport)
#else
#define ASSET_LOADER_DLL __declspec(dllimport)
#endif