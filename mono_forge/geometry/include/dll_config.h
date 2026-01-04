#pragma once

#ifdef geometry_EXPORTS
#define GEOMETRY_DLL __declspec(dllexport)
#else
#define GEOMETRY_DLL __declspec(dllimport)
#endif