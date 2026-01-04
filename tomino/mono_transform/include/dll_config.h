#pragma once

#ifdef MONOTRANSFORM_EXPORTS
#define MONO_TRANSFORM_API __declspec(dllexport)
#else
#define MONO_TRANSFORM_API __declspec(dllimport)
#endif