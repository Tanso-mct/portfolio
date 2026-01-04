#include "directx12_util/src/pch.h"
#include "directx12_util/include/helper.h"

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"

using namespace Microsoft::WRL;

namespace dx12_util
{

DX12_UTIL_DLL bool WaitForGpu(
    ID3D12Device4* device, ID3D12CommandQueue* command_queue)
{
    // Create a fence
    std::unique_ptr<Fence> fence 
        = Fence::CreateInstance<Fence>(0, D3D12_FENCE_FLAG_NONE, 100000, device);
    if (!fence)
        return false; // Failed to create fence

    // Signal the fence
    if (!fence->Signal(command_queue))
        return false; // Failed to signal fence

    // Wait for the fence
    if (!fence->Wait())
        return false; // Failed to wait for fence

    return true; // Success
}

DX12_UTIL_DLL bool LoadCSO(const char* file_path, ShaderBinary& out_shader_binary)
{
    FILE* fp = nullptr;
	errno_t error;

	error = fopen_s(&fp, file_path, "rb");
	if (error != 0)
    {
        utility_header::ConsoleLogErr(
            {"Failed to open CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Failed to open file
    }

	// Move the file pointer to the end to get the size
	error = fseek(fp, 0L, SEEK_END);
    if (error != 0)
    {
        utility_header::ConsoleLogErr(
            {"Failed to seek to end of CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        fclose(fp);
        return false; // Failed to seek
    }

	error = fgetpos(fp, &out_shader_binary.size);
    if (error != 0)
    {
        utility_header::ConsoleLogErr(
            {"Failed to get position in CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        fclose(fp);
        return false; // Failed to get position
    }

	// Move the file pointer back to the beginning
	error = fseek(fp, 0L, SEEK_SET);
    if (error != 0)
    {
        utility_header::ConsoleLogErr(
            {"Failed to seek to beginning of CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        fclose(fp);
        return false; // Failed to seek
    }

	out_shader_binary.data = std::make_unique<unsigned char[]>(out_shader_binary.size);

	// Read the file data into the buffer
	error = fread(out_shader_binary.data.get(), 1, out_shader_binary.size, fp);
    if (error != out_shader_binary.size)
    {
        utility_header::ConsoleLogErr(
            {"Failed to read CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        fclose(fp);
        return false; // Failed to read file
    }

    // Close the file
	error = fclose(fp);
    if (error != 0)
    {
        utility_header::ConsoleLogErr(
            {"Failed to close CSO file: ", file_path},
            __FILE__, __LINE__, __FUNCTION__);
        return false; // Failed to close file
    }

    return true; // Success
}

DX12_UTIL_DLL UINT GetDXGIFormatPixelSize(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 16;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
        return 8;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
        return 4;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
        return 2;

    default:
        assert(false && "Unsupported DXGI format for pixel size retrieval.");
    }
}

} // namespace dx12_util