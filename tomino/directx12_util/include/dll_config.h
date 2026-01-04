#pragma once

#ifdef directx12_util_EXPORTS
#define DX12_UTIL_DLL __declspec(dllexport)
#else
#define DX12_UTIL_DLL __declspec(dllimport)
#endif