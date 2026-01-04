#pragma once

#ifdef IMGUI_EXPORTS
#  define IMGUI_DLL __declspec(dllexport)
#else
#  define IMGUI_DLL __declspec(dllimport)
#endif