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

render_graph::MaterialTypeHandleID GraphicsServiceView::GetMaterialType(
    const render_graph::MaterialHandle* material_handle) const
{
    // Get graphics service API
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
        "GraphicsServiceAPI must be derived from ServiceAPI.");
    const GraphicsServiceAPI& graphics_service_api = dynamic_cast<const GraphicsServiceAPI&>(service_api_);

    // Get the material
    const render_graph::Material* material = graphics_service_api.GetMaterial(material_handle);
    assert(material != nullptr && "Material is null for the given material handle.");

    // Return the material type handle ID
    return material->GetMaterialTypeHandleID();
}

} // namespace mono_graphics_service