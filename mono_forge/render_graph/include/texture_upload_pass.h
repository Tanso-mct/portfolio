#pragma once

#include <vector>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"

namespace render_graph
{

// Render pass handle for TextureUploadPass
class RENDER_GRAPH_DLL TextureUploadPassHandle : public RenderPassHandle<TextureUploadPassHandle> {};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL TextureUploadPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        TextureUploadPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<>>
{
public:
    TextureUploadPass();
    ~TextureUploadPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    struct UploadTask
    {
        // Handle to the texture resource to be updated
        const ResourceHandle* texture_handle = nullptr;

        // Handle to the upload buffer resource
        const ResourceHandle* upload_buffer_handle = nullptr;
        
        // Pointer to the texture data to upload
        const void* data = nullptr;
    };

    // Add a texture upload task to the pass
    bool AddUploadTask(UploadTask&& task);

private:
    // List of texture upload tasks
    std::vector<UploadTask> tasks_;
};

} // namespace render_graph