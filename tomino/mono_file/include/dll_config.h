#pragma once

#ifdef MONOFILE_EXPORTS
#define MONO_FILE_API __declspec(dllexport)
#else
#define MONO_FILE_API __declspec(dllimport)
#endif