#pragma once

#ifdef mono_graphics_service_EXPORTS
#define MONO_GRAPHICS_SERVICE_DLL __declspec(dllexport)
#else
#define MONO_GRAPHICS_SERVICE_DLL __declspec(dllimport)
#endif