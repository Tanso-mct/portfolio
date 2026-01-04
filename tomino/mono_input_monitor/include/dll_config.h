#pragma once

#ifdef MONOINPUTMONITOR_EXPORTS
#define MONO_INPUT_MONITOR_API __declspec(dllexport)
#else
#define MONO_INPUT_MONITOR_API __declspec(dllimport)
#endif