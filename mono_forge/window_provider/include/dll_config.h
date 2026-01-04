#pragma once

#ifdef window_provider_EXPORTS
#define WINDOW_PROVIDER_DLL __declspec(dllexport)
#else
#define WINDOW_PROVIDER_DLL __declspec(dllimport)
#endif