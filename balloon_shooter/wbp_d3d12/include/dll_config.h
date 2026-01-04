#pragma once

#ifdef WBPD3D12_EXPORTS
#define WBP_D3D12_API __declspec(dllexport)
#else
#define WBP_D3D12_API __declspec(dllimport)
#endif