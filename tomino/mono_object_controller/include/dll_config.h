#pragma once

#ifdef MONOOBJECTCONTROLLER_EXPORTS
#define MONO_OBJECT_CONTROLLER_API __declspec(dllexport)
#else
#define MONO_OBJECT_CONTROLLER_API __declspec(dllimport)
#endif