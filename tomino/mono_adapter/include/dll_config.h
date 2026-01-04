#pragma once

#ifdef MONOADAPTER_EXPORTS
#define MONO_ADAPTER_API __declspec(dllexport)
#else
#define MONO_ADAPTER_API __declspec(dllimport)
#endif