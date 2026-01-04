#pragma once

#ifdef material_editor_EXPORTS
#define MATERIAL_EDITOR_DLL __declspec(dllexport)
#else
#define MATERIAL_EDITOR_DLL __declspec(dllimport)
#endif