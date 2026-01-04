#pragma once

#ifdef MONOIDENTITY_EXPORTS
#define MONO_IDENTITY_API __declspec(dllexport)
#else
#define MONO_IDENTITY_API __declspec(dllimport)
#endif