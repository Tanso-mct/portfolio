#include "render_graph/src/pch.h"
#include "render_graph/include/shadow_composition_pass.h"

#include "render_graph/include/heap_manager.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/light_manager.h"

namespace render_graph
{

ShadowCompositionPass::ShadowCompositionPass(std::unique_ptr<Pipeline> shadow_composition_pipeline)
    : shadow_composition_pipeline_(std::move(shadow_composition_pipeline))
{
    assert(shadow_composition_pipeline_ != nullptr && "Shadow composition pipeline is null.");
}

ShadowCompositionPass::~ShadowCompositionPass()
{
}

bool ShadowCompositionPass::Setup()
{
    // Setup pipeline
    if (!shadow_composition_pipeline_->Setup())
        return false;

    return true;
}

bool ShadowCompositionPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        ShadowCompositionPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            assert(fs_triangle_info_.vertex_buffer_handle->IsValid() && "Full-screen triangle vertex buffer handle is not set.");
            assert(fs_triangle_info_.index_buffer_handle->IsValid() && "Full-screen triangle index buffer handle is not set.");

            assert(
                render_target_handles_->size() == (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT && 
                "Render target handle count mismatch.");
            for (int i = 0; i < render_target_handles_->size(); ++i)
                assert(render_target_handles_->at(i).IsValid() && "Render target handle is not set.");
            
            assert(shadow_composition_config_buffer_handle_->IsValid() && "Shadow composition configuration buffer handle is not set.");
            assert(camera_inv_view_proj_matrix_buffer_handle_->IsValid() && "Camera inverse view-projection matrix buffer handle is not set.");
            assert(depth_texture_handle_->IsValid() && "Depth texture handle is not set.");

            for (int i = 0; i < render_light_handles_.size(); ++i)
            {
                const LightHandle* light_handle = render_light_handles_[i];
                assert(light_handle->IsValid() && "Light handle is not valid.");

                // Get light
                LightManager::GetInstance().WithLock([&](LightManager& light_manager)
                {
                    Light& light = light_manager.GetLight(light_handle);

                    assert(light.GetShadowMapHandles()->size() > frame_index && "Shadow map handle index out of range.");
                    assert(light.GetViewMatrixBufferHandle()->IsValid() && "Drawing light view matrix buffer handle is not set.");
                    assert(light.GetProjMatrixBufferHandle()->IsValid() && "Drawing light projection matrix buffer handle is not set.");
                });
            }

            builder.Read(fs_triangle_info_.vertex_buffer_handle); // Declare read access to full-screen triangle vertex buffer
            builder.Read(fs_triangle_info_.index_buffer_handle); // Declare read access to full-screen triangle index buffer
            builder.Read(shadow_composition_config_buffer_handle_); // Declare read access to shadow composition configuration buffer
            builder.Read(camera_inv_view_proj_matrix_buffer_handle_); // Declare read access to camera inverse view-projection matrix buffer

            for (int i = 0; i < render_light_handles_.size(); ++i)
            {
                const LightHandle* light_handle = render_light_handles_[i];

                // Get light
                LightManager::GetInstance().WithLock([&](LightManager& light_manager)
                {
                    Light& light = light_manager.GetLight(light_handle);

                    // Get shadow map texture handle
                    const ResourceHandle* shadow_map_texture_handle = &light.GetShadowMapHandles()->at(frame_index);

                    // Declare read access to shadow map texture
                    builder.Read(shadow_map_texture_handle);

                    // Declare read access to drawing light view matrix buffer
                    builder.Read(light.GetViewMatrixBufferHandle());

                    // Declare read access to drawing light projection matrix buffer
                    builder.Read(light.GetProjMatrixBufferHandle());
                });
            }

            builder.Read(depth_texture_handle_); // Declare read access to depth texture
            builder.Read(normal_texture_handle_); // Declare read access to normal texture

            for (int i = 0; i < render_target_handles_->size(); ++i)
                builder.Write(&render_target_handles_->at(i)); // Declare write access to render targets

            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            assert(has_viewport_ && "Viewport is not set.");
            assert(has_scissor_rect_ && "Scissor rect is not set.");

            // Get write access token
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();
            current_write_token_ = self_pass.GetWriteToken();

            // Get read access token
            const ResourceAccessToken& read_token = self_pass.GetReadToken();
            current_read_token_ = read_token;

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Set pipeline
            shadow_composition_pipeline_->SetPipeline(command_list.Get());

            // Set viewport and scissor rect
            command_list.Get()->RSSetViewports(1, &viewport_);
            command_list.Get()->RSSetScissorRects(1, &scissor_rect_);

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

            ResourceManager::GetInstance().WithLock([&](ResourceManager& resource_manager)
            {
                // Get render targets
                dx12_util::Texture2D* render_target_textures[(uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT];
                for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                {
                    // Get render target texture
                    dx12_util::Resource& render_target_resource
                        = resource_manager.GetWriteResource(&render_target_handles_->at(i), write_token);
                    dx12_util::Texture2D* render_target_texture
                        = dynamic_cast<dx12_util::Texture2D*>(&render_target_resource);
                    assert(render_target_texture != nullptr && "Final color buffer is not a Texture2D.");

                    // Store render target texture
                    render_target_textures[i] = render_target_texture;
                }

                // Set before state to render target barriers
                for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                {
                    dx12_util::Texture2D* final_color_texture = render_target_textures[i];

                    if (final_color_texture->GetCurrentState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
                    {
                        dx12_util::Barrier barrier_before_rt(
                            final_color_texture->Get(), command_list.Get(), 
                            final_color_texture->GetCurrentState(), D3D12_RESOURCE_STATE_RENDER_TARGET);
                        final_color_texture->SetCurrentState(D3D12_RESOURCE_STATE_RENDER_TARGET);
                    }
                }

                // Create RTV handle array
                D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[(uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT];
                for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                    rtv_handles[i] = render_target_textures[i]->GetRtvCpuHandle();

                // Set render target
                command_list.Get()->OMSetRenderTargets(
                    (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT, rtv_handles, FALSE, nullptr);

                // Clear render targets
                for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                {
                    float clear_color[4] = 
                    {
                        shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][0],
                        shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][1],
                        shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][2],
                        shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][3]
                    };

                    command_list.Get()->ClearRenderTargetView(rtv_handles[i], clear_color, 0, nullptr);
                }

                // Get vertex buffer
                const dx12_util::Resource& vertex_buffer_resource
                    = resource_manager.GetReadResource(fs_triangle_info_.vertex_buffer_handle, read_token);
                const dx12_util::Buffer& vertex_buffer
                    = dynamic_cast<const dx12_util::Buffer&>(vertex_buffer_resource);

                // Create vertex buffer view
                D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
                vertex_buffer_view.BufferLocation = vertex_buffer.GetGPUVirtualAddress();
                vertex_buffer_view.SizeInBytes = vertex_buffer.GetSize();
                vertex_buffer_view.StrideInBytes = sizeof(shadow_composition_pass::Vertex);

                // Set vertex buffer view
                command_list.Get()->IASetVertexBuffers(0, 1, &vertex_buffer_view);

                // Get index buffer
                const dx12_util::Resource& index_buffer_resource
                    = resource_manager.GetReadResource(fs_triangle_info_.index_buffer_handle, read_token);
                const dx12_util::Buffer& index_buffer
                    = dynamic_cast<const dx12_util::Buffer&>(index_buffer_resource);

                // Create index buffer view
                D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
                index_buffer_view.BufferLocation = index_buffer.GetGPUVirtualAddress();
                index_buffer_view.SizeInBytes = index_buffer.GetSize();
                index_buffer_view.Format = shadow_composition_pass::INDEX_BUFFER_FORMAT;

                // Set index buffer view
                command_list.Get()->IASetIndexBuffer(&index_buffer_view);

                for (int i = 0; i < render_light_handles_.size(); ++i)
                {
                    const LightHandle* light_handle = render_light_handles_[i];
                    assert(light_handle->IsValid() && "Light handle is not valid.");
    
                    LightManager::GetInstance().WithLock([&](LightManager& light_manager)
                    {
                        // Get light
                        Light& light = light_manager.GetLight(light_handle);

                        // Store shadow map texture handle
                        shadow_map_texture_handle_ = &light.GetShadowMapHandles()->at(frame_index);

                        // Store drawing light view matrix buffer handle
                        drawing_light_view_matrix_buffer_handle_ = light.GetViewMatrixBufferHandle();

                        // Store drawing light projection matrix buffer handle
                        drawing_light_proj_matrix_buffer_handle_ = light.GetProjMatrixBufferHandle();
                    });

                    // Set root parameters
                    shadow_composition_pipeline_->SetRootParameters(command_list.Get(), GetPassAPI());

                    // Draw call
                    command_list.Get()->DrawIndexedInstanced(fs_triangle_info_.index_count, 1, 0, 0, 0);
                }

                // Set render target to pixel shader resource barriers
                for (uint32_t i = 0; i < (uint32_t)shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                {
                    dx12_util::Barrier rt_to_pr_barrier(
                        render_target_textures[i]->Get(), command_list.Get(),
                        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                    // Set current state to pixel shader resource
                    render_target_textures[i]->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                }
            });

            // Clear infos
            fs_triangle_info_ = FullScreenTriangleInfo();
            render_target_handles_ = nullptr;
            render_light_handles_.clear();
            shadow_composition_config_buffer_handle_ = nullptr;
            camera_inv_view_proj_matrix_buffer_handle_ = nullptr;
            depth_texture_handle_ = nullptr;

            return true; // Execution successful
        }
    );
}

void ShadowCompositionPass::SetFullScreenTriangleInfo(FullScreenTriangleInfo&& fs_triangle_info)
{
    assert(IsSetup() && "Instance is not setup");
    fs_triangle_info_ = std::move(fs_triangle_info);
}

void ShadowCompositionPass::AddShadowCastingLight(const LightHandle* light_handle)
{
    assert(IsSetup() && "Instance is not setup");
    render_light_handles_.emplace_back(light_handle);
}

void ShadowCompositionPass::SetRenderTargetHandles(const ResourceHandles* texture_handles)
{
    assert(IsSetup() && "Instance is not setup");
    render_target_handles_ = texture_handles;
}

void ShadowCompositionPass::SetShadowCompositionConfigBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    shadow_composition_config_buffer_handle_ = buffer_handle;
}

void ShadowCompositionPass::SetCameraInvViewProjMatrixBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    camera_inv_view_proj_matrix_buffer_handle_ = buffer_handle;
}

void ShadowCompositionPass::SetDepthTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    depth_texture_handle_ = texture_handle;
}

void ShadowCompositionPass::SetNormalTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    normal_texture_handle_ = texture_handle;
}

void ShadowCompositionPass::SetShadowCompositionConfig(shadow_composition_pass::ShadowCompositionConfigBuffer config)
{
    assert(IsSetup() && "Instance is not setup");
    shadow_composition_config_ = config;
}

const shadow_composition_pass::ShadowCompositionConfigBuffer& ShadowCompositionPass::GetShadowCompositionConfig() const
{
    assert(IsSetup() && "Instance is not setup");
    return shadow_composition_config_;
}

void ShadowCompositionPass::SetViewport(const D3D12_VIEWPORT& viewport)
{
    assert(IsSetup() && "Instance is not setup");
    viewport_ = viewport;
    has_viewport_ = true;
}

void ShadowCompositionPass::SetScissorRect(const D3D12_RECT& scissor_rect)
{
    assert(IsSetup() && "Instance is not setup");
    scissor_rect_ = scissor_rect;
    has_scissor_rect_ = true;
}

const ResourceHandle* ShadowCompositionPass::GetShadowCompositionConfigBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return shadow_composition_config_buffer_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetShadowMapTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return shadow_map_texture_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetDrawingLightViewMatrixBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return drawing_light_view_matrix_buffer_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetDrawingLightProjMatrixBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return drawing_light_proj_matrix_buffer_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetCameraInvViewProjMatrixBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return camera_inv_view_proj_matrix_buffer_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetDepthTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return depth_texture_handle_;
}

const ResourceHandle* ShadowCompositionPass::GetNormalTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return normal_texture_handle_;
}

const ResourceAccessToken& ShadowCompositionPass::GetReadAccessToken() const
{
    assert(IsSetup() && "Instance is not setup");
    return current_read_token_;
}

const ResourceAccessToken& ShadowCompositionPass::GetWriteAccessToken() const
{
    assert(IsSetup() && "Instance is not setup");
    return current_write_token_;
}

} // namespace render_graph