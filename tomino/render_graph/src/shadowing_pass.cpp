#include "render_graph/src/pch.h"
#include "render_graph/include/shadowing_pass.h"

#include "render_graph/include/light_manager.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/heap_manager.h"

namespace render_graph
{

ShadowingPass::ShadowingPass(std::unique_ptr<Pipeline> shadowing_pipeline)
    : shadowing_pipeline_(std::move(shadowing_pipeline))
{
    assert(shadowing_pipeline_ != nullptr && "Shadowing pipeline is null.");
}

ShadowingPass::~ShadowingPass()
{
}

bool ShadowingPass::Setup()
{
    // Setup shadowing pipeline
    if (!shadowing_pipeline_->Setup())
        return false;
    
    return true;
}

bool ShadowingPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        ShadowingPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            for (int i = 0; i < render_light_handles_.size(); ++i)
                assert(render_light_handles_.at(i)->IsValid() && "Invalid light handle added to ShadowingPass");

            render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& light_manager) 
            {
                for (int i = 0; i < render_light_handles_.size(); ++i)
                {
                    const LightHandle* light_handle = render_light_handles_.at(i);
                    const Light& light = light_manager.GetLight(light_handle);
                    const ResourceHandles* shadow_map_handles = light.GetShadowMapHandles();
                    builder.Write(&shadow_map_handles->at(current_frame_index_)); // Declare write access to shadow map

                    assert(light.GetViewMatrixBufferHandle()->IsValid() && "Light view matrix buffer handle is invalid.");
                    builder.Read(light.GetViewMatrixBufferHandle()); // Declare read access to light view matrix buffer 

                    assert(light.GetProjMatrixBufferHandle()->IsValid() && "Light projection matrix buffer handle is invalid.");
                    builder.Read(light.GetProjMatrixBufferHandle()); // Declare read access to light projection matrix buffer
                }

                // Declare read access to world matrix buffers of all mesh infos
                for (const MeshInfo& mesh_info : mesh_infos_)
                {
                    assert(mesh_info.world_matrix_buffer_handle->IsValid() && "World matrix buffer handle is invalid.");
                    builder.Read(mesh_info.world_matrix_buffer_handle); // Declare read access to world matrix buffer
                    builder.Read(mesh_info.vertex_buffer_handle); // Declare read access to vertex buffer
                    builder.Read(mesh_info.index_buffer_handle); // Declare read access to index buffer
                }
            });

            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            if (render_light_handles_.empty() || mesh_infos_.empty())
                return true; // Nothing to render

            // Get write access tokens
            ResourceAccessToken& write_token = self_pass.GetWriteToken();
            current_write_token_ = write_token;

            // Get read access tokens
            const ResourceAccessToken& read_token = self_pass.GetReadToken();
            current_read_token_ = read_token;

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Set primitive topology
            command_list.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // Create descriptor heaps vector
            std::vector<ID3D12DescriptorHeap*> descriptor_heaps;
            HeapManager::GetInstance().WithUniqueLock([&](HeapManager& heap_manager) 
            {
                descriptor_heaps.push_back(heap_manager.GetSrvHeap().Get());
            });

            // Set descriptor heaps
            command_list.Get()->SetDescriptorHeaps(descriptor_heaps.size(), descriptor_heaps.data());

            for (int i = 0; i < render_light_handles_.size(); ++i)
            {
                const LightHandle* light_handle = render_light_handles_.at(i);

                const ResourceHandle* shadow_map_handle = nullptr;
                D3D12_VIEWPORT viewport = {};
                D3D12_RECT scissor_rect = {};
                LightManager::GetInstance().WithLock([&](LightManager& light_manager) 
                {
                    // Get light
                    const Light& light = light_manager.GetLight(light_handle);

                    // Set drawing view matrix buffer handle
                    drawing_light_view_matrix_buffer_handle_ = light.GetViewMatrixBufferHandle();

                    // Set drawing projection matrix buffer handle
                    drawing_light_proj_matrix_buffer_handle_ = light.GetProjMatrixBufferHandle();

                    // Get shadow map handle
                    shadow_map_handle = &light.GetShadowMapHandles()->at(current_frame_index_);

                    // Get viewport and scissor rect
                    viewport = light.GetViewport();
                    scissor_rect = light.GetScissorRect();
                });

                D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = D3D12_CPU_DESCRIPTOR_HANDLE();
                ResourceManager::GetInstance().WithLock([&](ResourceManager& resource_manager) 
                {
                    // Get shadow map texture
                    dx12_util::Resource& shadow_map_texture_resource
                        = resource_manager.GetWriteResource(shadow_map_handle, write_token); 
                    dx12_util::Texture2D* shadow_map_texture 
                        = dynamic_cast<dx12_util::Texture2D*>(&shadow_map_texture_resource);
                    assert(shadow_map_texture != nullptr && "Failed to get shadow map texture.");

                    // Set before to depth write state barrier
                    if (shadow_map_texture->GetCurrentState() != D3D12_RESOURCE_STATE_DEPTH_WRITE)
                    {
                        dx12_util::Barrier to_depth_write_barrier(
                            shadow_map_texture->Get(), command_list.Get(),
                            shadow_map_texture->GetCurrentState(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

                        // Set current state to depth write
                        shadow_map_texture->SetCurrentState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
                    }

                    // Get DSV handle
                    dsv_handle = shadow_map_texture->GetDsvCpuHandle();
                });

                // Set viewport and scissor rect
                command_list.Get()->RSSetViewports(1, &viewport);
                command_list.Get()->RSSetScissorRects(1, &scissor_rect);

                // Set pipeline
                shadowing_pipeline_->SetPipeline(command_list.Get());

                // Set render targets
                command_list.Get()->OMSetRenderTargets(0, nullptr, FALSE, &dsv_handle);

                // Clear depth stencil
                command_list.Get()->ClearDepthStencilView(
                    dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 
                    shadowing_pass::SHADOW_MAP_CLEAR_VALUE, shadowing_pass::SHADOW_MAP_STENCIL_CLEAR_VALUE, 0, &scissor_rect);

                // Render each mesh buffer
                for (const MeshInfo& mesh_info : mesh_infos_)
                {
                    ResourceManager::GetInstance().WithLock([&](ResourceManager& resource_manager) 
                    {
                        // Set drawing world matrix buffer handle
                        drawing_world_matrix_buffer_handle_ = mesh_info.world_matrix_buffer_handle;

                        // Set root parameters
                        shadowing_pipeline_->SetRootParameters(command_list.Get(), GetPassAPI());

                        // Get vertex buffer
                        const dx12_util::Resource& vertex_buffer_resource
                            = resource_manager.GetReadResource(mesh_info.vertex_buffer_handle, read_token);
                        const dx12_util::Buffer* vertex_buffer = dynamic_cast<const dx12_util::Buffer*>(&vertex_buffer_resource);
                        assert(vertex_buffer != nullptr && "Failed to get vertex buffer.");

                        // Create vertex buffer view
                        D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
                        vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
                        vertex_buffer_view.SizeInBytes = vertex_buffer->GetSize();
                        vertex_buffer_view.StrideInBytes = sizeof(shadowing_pass::Vertex);

                        // Set vertex buffer view
                        command_list.Get()->IASetVertexBuffers(0, 1, &vertex_buffer_view);

                        // Get index buffer
                        const dx12_util::Resource& index_buffer_resource
                            = resource_manager.GetReadResource(mesh_info.index_buffer_handle, read_token);
                        const dx12_util::Buffer& index_buffer 
                            = dynamic_cast<const dx12_util::Buffer&>(index_buffer_resource);

                        // Create index buffer view
                        D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
                        index_buffer_view.BufferLocation = index_buffer.GetGPUVirtualAddress();
                        index_buffer_view.SizeInBytes = index_buffer.GetSize();
                        index_buffer_view.Format = shadowing_pass::INDEX_BUFFER_FORMAT;

                        // Set index buffer view
                        command_list.Get()->IASetIndexBuffer(&index_buffer_view);

                        // Draw call
                        command_list.Get()->DrawIndexedInstanced(mesh_info.index_count, 1, 0, 0, 0);
                    });
                }

                ResourceManager::GetInstance().WithLock([&](ResourceManager& resource_manager) 
                {
                    // Get shadow map texture
                    dx12_util::Resource& shadow_map_texture_resource
                        = resource_manager.GetWriteResource(shadow_map_handle, write_token);
                    dx12_util::Texture2D* shadow_map_texture 
                        = dynamic_cast<dx12_util::Texture2D*>(&shadow_map_texture_resource);
                    assert(shadow_map_texture != nullptr && "Failed to get shadow map texture.");

                    // Set shadow map to pixel shader resource state barrier
                    dx12_util::Barrier depth_to_pr_barrier(
                        shadow_map_texture->Get(), command_list.Get(),
                        D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                    // Set current state to pixel shader resource
                    shadow_map_texture->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                });
            }

            // Clear handles after rendering
            render_light_handles_.clear();
            drawing_light_view_matrix_buffer_handle_ = nullptr;
            drawing_light_proj_matrix_buffer_handle_ = nullptr;
            drawing_world_matrix_buffer_handle_ = nullptr;

            // Clear mesh infos after rendering
            mesh_infos_.clear();

            return true; // Execution successful
        }
    );
}

void ShadowingPass::AddShadowCasterMeshInfo(MeshInfo&& mesh_buffer_info)
{
    mesh_infos_.emplace_back(std::move(mesh_buffer_info));
}

void ShadowingPass::AddShadowCastingLight(const LightHandle* light_handle)
{
    render_light_handles_.emplace_back(light_handle);
}

const ResourceHandle* ShadowingPass::GetDrawingLightViewMatrixBufferHandle() const
{
    assert(drawing_light_view_matrix_buffer_handle_->IsValid() && "Drawing light view matrix buffer handle is invalid.");
    return drawing_light_view_matrix_buffer_handle_;
}

const ResourceHandle* ShadowingPass::GetDrawingLightProjMatrixBufferHandle() const
{
    assert(drawing_light_proj_matrix_buffer_handle_->IsValid() && "Drawing light projection matrix buffer handle is invalid.");
    return drawing_light_proj_matrix_buffer_handle_;
}

const ResourceHandle* ShadowingPass::GetDrawingWorldMatrixBufferHandle() const
{
    assert(drawing_world_matrix_buffer_handle_->IsValid() && "Drawing world matrix buffer handle is invalid.");
    return drawing_world_matrix_buffer_handle_;
}

const ResourceAccessToken& ShadowingPass::GetCurrentReadToken() const
{
    return current_read_token_;
}

const ResourceAccessToken& ShadowingPass::GetCurrentWriteToken() const
{
    return current_write_token_;
}

} // namespace render_graph