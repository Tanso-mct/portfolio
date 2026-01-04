#pragma once

#include "mono_service/include/service_view.h"

#include "mono_graphics_service/include/dll_config.h"
#include "mono_graphics_service/include/graphics_service.h"

namespace mono_graphics_service
{

class MONO_GRAPHICS_SERVICE_DLL GraphicsServiceView :
    public mono_service::ServiceView
{
public:
    GraphicsServiceView(const mono_service::ServiceAPI& service_api) :
        ServiceView(service_api)
    {
    }

    virtual ~GraphicsServiceView() override = default;

    // Get the current back buffer index
    UINT GetCurrentBackBufferIndex(const render_graph::ResourceHandle* swap_chain_handle) const;

    // Get the shader resource view descriptor handle for a resource
    D3D12_GPU_DESCRIPTOR_HANDLE GetTexture2DSRVHandle(const render_graph::ResourceHandle* resource_handle) const;
};

} // namespace mono_graphics_service