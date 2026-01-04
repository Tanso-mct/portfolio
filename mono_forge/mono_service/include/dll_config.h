#pragma once

#ifdef mono_service_EXPORTS
#define MONO_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_SERVICE_DLL __declspec(dllimport)
#endif