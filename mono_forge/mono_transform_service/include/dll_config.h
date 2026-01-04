#pragma once

#ifdef mono_transform_service_EXPORTS
#define MONO_TRANSFORM_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_TRANSFORM_SERVICE_DLL __declspec(dllimport)
#endif