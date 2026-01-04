#include "render_graph/src/pch.h"
#include "render_graph/include/buffer_upload_pass.h"

#include "render_graph/include/resource_manager.h"

namespace render_graph
{

BufferUploadPass::BufferUploadPass()
{
}

BufferUploadPass::~BufferUploadPass()
{
}

bool BufferUploadPass::Setup()
{
    // No special setup needed for buffer update pass
    return true;
}

bool BufferUploadPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        BufferUploadPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            for (const UploadTask& task : tasks_)
                builder.Write(task.buffer_handle); // Declare write access for each buffer

            for (const StructuredBufferUploadTask& task : structured_buffer_tasks_)
            {
                builder.Write(task.buffer_handle); // Declare write access for each buffer with upload buffer
                builder.Read(task.upload_buffer_handle); // Declare read access for each upload buffer
            }

            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            if (tasks_.empty())
                return true; // No tasks to execute, return success
                
            // Get write access token
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();

            // Get read access token
            const ResourceAccessToken& read_token = self_pass.GetReadToken();

            // Get command list from context
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Update each buffer
            for (const UploadTask& task : tasks_)
            {
                ResourceManager& resource_manager = ResourceManager::GetInstance();
                resource_manager.WithLock([&](ResourceManager& manager)
                {
                    // Get the buffer resource for writing
                    dx12_util::Resource& resource = manager.GetWriteResource(task.buffer_handle, write_token);

                    // Cast to dx12_util::Buffer
                    dx12_util::Buffer* buffer = dynamic_cast<dx12_util::Buffer*>(&resource);
                    assert(buffer != nullptr); // Ensure the cast succeeded

                    // Update the buffer data
                    bool result = buffer->UpdateData(task.data, task.size);
                    assert(result); // Ensure the update succeeded
                });
            }

            // Update each buffer with upload buffer
            for (const StructuredBufferUploadTask& task : structured_buffer_tasks_)
            {
                ResourceManager& resource_manager = ResourceManager::GetInstance();
                resource_manager.WithLock([&](ResourceManager& manager)
                {
                    // Get the buffer resource for writing
                    dx12_util::Resource& resource = manager.GetWriteResource(task.buffer_handle, write_token);

                    // Cast to dx12_util::StructuredBuffer
                    dx12_util::StructuredBuffer* buffer = dynamic_cast<dx12_util::StructuredBuffer*>(&resource);
                    assert(buffer != nullptr); // Ensure the cast succeeded

                    // Get the upload buffer resource for writing
                    const dx12_util::Resource& upload_resource = manager.GetReadResource(task.upload_buffer_handle, read_token);
                    const dx12_util::Buffer* upload_buffer = dynamic_cast<const dx12_util::Buffer*>(&upload_resource);
                    assert(upload_buffer != nullptr); // Ensure the cast succeeded

                    // Transition structured buffer to COPY_DEST state
                    dx12_util::Barrier barrier_to_copy_dest(
                            buffer->Get(), command_list.Get(),
                            buffer->GetCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
                    buffer->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);

                    // Copy data from upload buffer to structured buffer
                    command_list.Get()->CopyBufferRegion(
                        buffer->Get(), 0, upload_buffer->Get(), 0, task.size);

                    // Transition structured buffer back to GENERIC_READ state
                    dx12_util::Barrier barrier_to_generic_read(
                            buffer->Get(), command_list.Get(),
                            buffer->GetCurrentState(), D3D12_RESOURCE_STATE_GENERIC_READ);
                    buffer->SetCurrentState(D3D12_RESOURCE_STATE_GENERIC_READ);
                });
            }

            // Clear tasks after execution
            tasks_.clear();
            structured_buffer_tasks_.clear();

            return true; // Execution successful
        }
    );
}

bool BufferUploadPass::AddUploadTask(UploadTask &&task)
{
    assert(IsSetup() && "Instance is not setup");
    tasks_.emplace_back(std::move(task));
    return true;
}

bool BufferUploadPass::AddUploadTask(StructuredBufferUploadTask &&task)
{
    assert(IsSetup() && "Instance is not setup");
    structured_buffer_tasks_.emplace_back(std::move(task));
    return true;
}

} // namespace render_graph