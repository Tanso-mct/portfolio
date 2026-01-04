#include "mono_service/src/pch.h"
#include "mono_graphics_service/include/graphics_service_view.h"

namespace mono_graphics_service
{

UINT GraphicsServiceView::GetCurrentBackBufferIndex(const render_graph::ResourceHandle* swap_chain_handle) const
{
    // Get graphics service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
        "GraphicsServiceAPI must be derived from ServiceAPI.");
    const GraphicsServiceAPI& graphics_service_api = dynamic_cast<const GraphicsServiceAPI&>(service_api_);

    // Return the current back buffer index
    return graphics_service_api.GetCurrentBackBufferIndex(swap_chain_handle);
}

D3D12_GPU_DESCRIPTOR_HANDLE GraphicsServiceView::GetTexture2DSRVHandle(
    const render_graph::ResourceHandle* resource_handle) const
{
    // Get graphics service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
        "GraphicsServiceAPI must be derived from ServiceAPI.");
    const GraphicsServiceAPI& graphics_service_api = dynamic_cast<const GraphicsServiceAPI&>(service_api_);

    // Return the texture 2D SRV handle
    return graphics_service_api.GetTexture2DSRVHandle(resource_handle);
}

} // namespace mono_graphics_service