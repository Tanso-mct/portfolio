#pragma once

#ifdef component_editor_EXPORTS
#define COMPONENT_EDITOR_DLL __declspec(dllexport)
#else
#define COMPONENT_EDITOR_DLL __declspec(dllimport)
#endif