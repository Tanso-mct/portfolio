#pragma once

#include <vector>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"

namespace render_graph
{

// Render pass handle for BufferUploadPass
class RENDER_GRAPH_DLL BufferUploadPassHandle : public RenderPassHandle<BufferUploadPassHandle> {};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL BufferUploadPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        BufferUploadPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<>>
{
public:
    BufferUploadPass();
    ~BufferUploadPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Structure for buffer update task
    struct UploadTask
    {
        // Buffer handle
        const ResourceHandle* buffer_handle = nullptr;

        // Data pointer
        const void* data;
        
        // Size of data
        uint32_t size;
    };

    // Update task for structured buffer with upload buffer
    struct StructuredBufferUploadTask
    {
        // Buffer handle
        const ResourceHandle* buffer_handle = nullptr;

        // Upload buffer handle
        const ResourceHandle* upload_buffer_handle = nullptr;

        // Size of data
        uint32_t size;
    };

    // Add a buffer update task to the pass
    bool AddUploadTask(UploadTask&& task);

    // Add a buffer update task with upload buffer to the pass
    bool AddUploadTask(StructuredBufferUploadTask&& task);

private:
    // List of buffer update tasks
    std::vector<UploadTask> tasks_;

    // List of structured buffer upload tasks
    std::vector<StructuredBufferUploadTask> structured_buffer_tasks_;
};

} // namespace render_graph