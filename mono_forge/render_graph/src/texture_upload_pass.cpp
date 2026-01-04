#include "render_graph/src/pch.h"
#include "render_graph/include/texture_upload_pass.h"

#include "directx12_util/include/helper.h"
#include "directx12_util/include/d3dx12.h"

namespace render_graph
{

TextureUploadPass::TextureUploadPass()
{
}

TextureUploadPass::~TextureUploadPass()
{
}

bool TextureUploadPass::Setup()
{
    // No special setup needed for buffer update pass
    return true;
}

bool TextureUploadPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        TextureUploadPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            for (const UploadTask& task : tasks_)
            {
                builder.Write(task.upload_buffer_handle);
                builder.Write(task.texture_handle);
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

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Iterate through each upload task
            for (const UploadTask& task : tasks_)
            {
                ResourceManager& resource_manager = ResourceManager::GetInstance();
                resource_manager.WithLock([&](ResourceManager& manager)
                {
                    // Get the upload buffer resource for writing
                    dx12_util::Resource& upload_resource 
                        = manager.GetWriteResource(task.upload_buffer_handle, write_token);

                    // Cast to dx12_util::Buffer
                    dx12_util::Buffer* upload_buffer = dynamic_cast<dx12_util::Buffer*>(&upload_resource);
                    assert(upload_buffer != nullptr); // Ensure the cast succeeded

                    // Get the texture resource for writing
                    dx12_util::Resource& texture_resource 
                        = manager.GetWriteResource(task.texture_handle, write_token);

                    // Cast to dx12_util::Texture2D
                    dx12_util::Texture2D* texture = dynamic_cast<dx12_util::Texture2D*>(&texture_resource);
                    assert(texture != nullptr); // Ensure the cast succeeded

                    // Create a subresource data structure to update the subresource data of a texture
                    const UINT subresourceCount = 1;
                    D3D12_SUBRESOURCE_DATA subresourceData = {};
                    subresourceData.pData = task.data;
                    subresourceData.RowPitch = texture->GetWidth() * dx12_util::GetDXGIFormatPixelSize(texture->GetFormat());
                    subresourceData.SlicePitch = subresourceData.RowPitch * texture->GetHeight();

                    // Create barrier to transition texture to COPY_DEST state
                    dx12_util::Barrier to_copy_dest_barrier(
                        texture->Get(), command_list.Get(),
                        texture->GetCurrentState(), D3D12_RESOURCE_STATE_COPY_DEST);
                    texture->SetCurrentState(D3D12_RESOURCE_STATE_COPY_DEST);
                    
                    // Use UpdateSubresources to copy data from the upload buffer to the texture
                    UpdateSubresources(
                        command_list.Get(), texture->Get(), upload_buffer->Get(),
                        0, 0, subresourceCount, &subresourceData);

                    // Create barrier to transition texture back to PIXEL_SHADER_RESOURCE state
                    dx12_util::Barrier to_pixel_shader_resource_barrier(
                        texture->Get(), command_list.Get(),
                        texture->GetCurrentState(), D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                    texture->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                });
            }

            // Clear tasks after execution
            tasks_.clear();

            return true; // Execution successful
        }
    );
}

bool TextureUploadPass::AddUploadTask(UploadTask&& task)
{
    assert(IsSetup() && "Instance is not setup");
    tasks_.emplace_back(std::move(task));
    return true;
}

} // namespace render_graph