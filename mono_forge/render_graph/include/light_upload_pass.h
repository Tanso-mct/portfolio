#pragma once

#include <vector>
#include <DirectXMath.h>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/light_handle.h"
#include "render_graph/include/light.h"

namespace render_graph
{

// Render pass handle for LightUploadPass
class RENDER_GRAPH_DLL LightUploadPassHandle : 
    public RenderPassHandle<LightUploadPassHandle> {};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL LightUploadPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        LightUploadPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<>>
{
public:
    LightUploadPass();
    ~LightUploadPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Add a light to be uploaded in this pass
    void AddUploadLight(const LightHandle* light_handle);

    // Set the upload buffer handle for lights
    void SetLightsUploadBufferHandle(const ResourceHandle* buffer_handle);

    // Set the light config buffer handle for lights
    void SetLightConfigBufferHandle(const ResourceHandle* buffer_handle);

    // Set the light configuration
    void SetLightConfig(Light::LightConfigBuffer config);

private:
    // List of light handles to be uploaded
    std::vector<const LightHandle*> light_handles_;

    // Upload buffer handle for lights
    const ResourceHandle* lights_upload_buffer_handle_ = nullptr;

    // Light config buffer handle
    const ResourceHandle* light_config_buffer_handle_ = nullptr;

    // Light configuration
    Light::LightConfigBuffer light_config_ = Light::LightConfigBuffer();
};

} // namespace render_graph