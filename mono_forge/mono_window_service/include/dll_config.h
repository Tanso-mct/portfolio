#pragma once

#ifdef mono_window_service_EXPORTS
#define MONO_WINDOW_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_WINDOW_SERVICE_DLL __declspec(dllimport)
#endif