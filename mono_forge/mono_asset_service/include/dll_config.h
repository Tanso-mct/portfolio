#pragma once

#ifdef mono_asset_service_EXPORTS
#define MONO_ASSET_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_ASSET_SERVICE_DLL __declspec(dllimport)
#endif