#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>

#include "directx12_util/include/dll_config.h" // DLL export/import macros

namespace dx12_util
{

// Waits for the GPU to finish processing all commands up to this point.
DX12_UTIL_DLL bool WaitForGpu(
    ID3D12Device4* device, ID3D12CommandQueue* command_queue);

// Represents a compiled shader binary.
struct ShaderBinary
{
    std::unique_ptr<unsigned char[]> data = nullptr;
    fpos_t size = 0;
};

// Loads a compiled shader object (CSO) file into a ShaderBinary structure.
DX12_UTIL_DLL bool LoadCSO(
    const char* file_path, ShaderBinary& out_shader_binary);

// Returns the size in bytes of a single pixel for the given DXGI format.
DX12_UTIL_DLL UINT GetDXGIFormatPixelSize(DXGI_FORMAT format);

} // namespace dx12_util