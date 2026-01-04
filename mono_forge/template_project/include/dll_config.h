#pragma once

#ifdef $project_name$_EXPORTS
#define $DLL_NAME$ __declspec(dllexport)
#else
#define $DLL_NAME$ __declspec(dllimport)
#endif