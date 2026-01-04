#pragma once

#ifdef MONOD3D12_EXPORTS
#define MONO_D3D12_API __declspec(dllexport)
#else
#define MONO_D3D12_API __declspec(dllimport)
#endif