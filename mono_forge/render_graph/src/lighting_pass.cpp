#include "render_graph/src/pch.h"
#include "render_graph/include/lighting_pass.h"

#include "render_graph/include/heap_manager.h"
#include "render_graph/include/resource_manager.h"

namespace render_graph
{

LightingPass::LightingPass()
{
}

LightingPass::~LightingPass()
{
}

bool LightingPass::Setup(std::unique_ptr<Pipeline> lighting_pipeline)
{
    lighting_pipeline_ = std::move(lighting_pipeline);
    assert(lighting_pipeline_ != nullptr && "Lighting pipeline must not be null");
    if (!lighting_pipeline_->Setup())
        return false;
    
    return true; // Setup successful
}

bool LightingPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        LightingPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            assert(full_screen_triangle_info_.vertex_buffer_handle->IsValid() && "Full-screen triangle vertex buffer handle is not set.");
            assert(full_screen_triangle_info_.index_buffer_handle->IsValid() && "Full-screen triangle index buffer handle is not set.");
            assert(inv_view_proj_matrix_buffer_handle_->IsValid() && "Inverse view-projection matrix buffer handle is not set.");
            assert(right_config_buffer_handle_->IsValid() && "Light configuration buffer handle is not set.");
            assert(rights_buffer_handle_->IsValid() && "Lights buffer handle is not set.");

            assert(
                render_target_texture_handles_->size() == (uint32_t)lighting_pass::RenderTargetIndex::COUNT && 
                "Final color buffer handles size mismatch.");
            for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                assert(render_target_texture_handles_->at(i).IsValid() && "Final color buffer handle is not set.");

            assert(albedo_texture_handle_->IsValid() && "Albedo texture handle is not set.");
            assert(normal_texture_handle_->IsValid() && "Normal texture handle is not set.");
            assert(metalness_texture_handle_->IsValid() && "Metalness texture handle is not set.");
            assert(roughness_texture_handle_->IsValid() && "Roughness texture handle is not set.");
            assert(specular_texture_handle_->IsValid() && "Specular texture handle is not set.");
            assert(ao_texture_handle_->IsValid() && "Ambient occlusion texture handle is not set.");
            assert(emission_texture_handle_->IsValid() && "Emission texture handle is not set.");
            assert(mask_material_texture_handle_->IsValid() && "Mask material texture handle is not set.");
            assert(mask_shadow_texture_handle_->IsValid() && "Mask shadow texture handle is not set.");
            assert(depth_stencil_texture_handle_->IsValid() && "Depth stencil texture handle is not set.");

            builder.Read(full_screen_triangle_info_.vertex_buffer_handle); // Declare read access to full-screen triangle vertex buffer
            builder.Read(full_screen_triangle_info_.index_buffer_handle); // Declare read access to full-screen triangle index buffer
            builder.Read(inv_view_proj_matrix_buffer_handle_); // Declare read access to inverse view-projection matrix buffer
            builder.Read(right_config_buffer_handle_); // Declare read access to light configuration buffer
            builder.Read(rights_buffer_handle_); // Declare read access to lights buffer
            builder.Read(albedo_texture_handle_); // Declare read access to albedo texture
            builder.Read(normal_texture_handle_); // Declare read access to normal texture
            builder.Read(metalness_texture_handle_); // Declare read access to metalness texture
            builder.Read(roughness_texture_handle_); // Declare read access to roughness texture
            builder.Read(specular_texture_handle_); // Declare read access to specular texture
            builder.Read(ao_texture_handle_); // Declare read access to ambient occlusion texture
            builder.Read(emission_texture_handle_); // Declare read access to emission texture
            builder.Read(mask_material_texture_handle_); // Declare read access to mask material texture
            builder.Read(mask_shadow_texture_handle_); // Declare read access to mask shadow texture
            builder.Read(depth_stencil_texture_handle_); // Declare read access to depth stencil buffer

            for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                builder.Write(&render_target_texture_handles_->at(i)); // Declare write access to final color buffer

            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            assert(has_viewport_ && "Viewport is not set.");
            assert(has_scissor_rect_ && "Scissor rect is not set.");

            // Get write access token
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();
            current_write_token_ = write_token;

            // Get read access token
            const ResourceAccessToken& read_token = self_pass.GetReadToken();
            current_read_token_ = read_token;

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Set pipeline
            lighting_pipeline_->SetPipeline(command_list.Get());

            // Set viewport and scissor rect
            command_list.Get()->RSSetViewports(1, &viewport_);
            command_list.Get()->RSSetScissorRects(1, &scissor_rect_);

            // Set primitive topology
            command_list.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            // Get heap manager
            HeapManager& heap_manager = HeapManager::GetInstance();

            // Create descriptor heaps vector
            std::vector<ID3D12DescriptorHeap*> descriptor_heaps;
            heap_manager.WithUniqueLock([&](HeapManager& manager) 
            {
                descriptor_heaps.push_back(manager.GetSrvHeap().Get());
            });

            // Set descriptor heaps
            command_list.Get()->SetDescriptorHeaps(descriptor_heaps.size(), descriptor_heaps.data());

            ResourceManager& resource_manager = ResourceManager::GetInstance();
            resource_manager.WithLock([&](ResourceManager& manager)
            {
                // Get render targets
                dx12_util::Texture2D* render_target_textures[(uint32_t)lighting_pass::RenderTargetIndex::COUNT];
                for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                {
                    // Get render target texture
                    dx12_util::Resource& render_target_resource
                        = manager.GetWriteResource(&render_target_texture_handles_->at(i), write_token);
                    dx12_util::Texture2D* render_target_texture
                        = dynamic_cast<dx12_util::Texture2D*>(&render_target_resource);
                    assert(render_target_texture != nullptr && "Final color buffer is not a Texture2D.");

                    // Store render target texture
                    render_target_textures[i] = render_target_texture;
                }

                // Set before state to render target barriers
                for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
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
                D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[(uint32_t)lighting_pass::RenderTargetIndex::COUNT];
                for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                    rtv_handles[i] = render_target_textures[i]->GetRtvCpuHandle();

                // Set render target
                command_list.Get()->OMSetRenderTargets(
                    (uint32_t)lighting_pass::RenderTargetIndex::COUNT, rtv_handles, FALSE, nullptr);

                // Clear render targets
                for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                {
                    float clear_color[4] = 
                    {
                        lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][0],
                        lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][1],
                        lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][2],
                        lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][3]
                    };

                    command_list.Get()->ClearRenderTargetView(rtv_handles[i], clear_color, 0, nullptr);
                }

                // Set root parameters
                lighting_pipeline_->SetRootParameters(command_list.Get(), GetPassAPI());

                // Get vertex buffer
                const dx12_util::Resource& vertex_buffer_resource
                    = manager.GetReadResource(full_screen_triangle_info_.vertex_buffer_handle, read_token);
                const dx12_util::Buffer& vertex_buffer
                    = dynamic_cast<const dx12_util::Buffer&>(vertex_buffer_resource);

                // Create vertex buffer view
                D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
                vertex_buffer_view.BufferLocation = vertex_buffer.GetGPUVirtualAddress();
                vertex_buffer_view.SizeInBytes = vertex_buffer.GetSize();
                vertex_buffer_view.StrideInBytes = sizeof(lighting_pass::Vertex);

                // Set vertex buffer view
                command_list.Get()->IASetVertexBuffers(0, 1, &vertex_buffer_view);

                // Get index buffer
                const dx12_util::Resource& index_buffer_resource
                    = manager.GetReadResource(full_screen_triangle_info_.index_buffer_handle, read_token);
                const dx12_util::Buffer& index_buffer
                    = dynamic_cast<const dx12_util::Buffer&>(index_buffer_resource);

                // Create index buffer view
                D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
                index_buffer_view.BufferLocation = index_buffer.GetGPUVirtualAddress();
                index_buffer_view.SizeInBytes = index_buffer.GetSize();
                index_buffer_view.Format = lighting_pass::INDEX_BUFFER_FORMAT;

                // Set index buffer view
                command_list.Get()->IASetIndexBuffer(&index_buffer_view);

                // Draw call
                command_list.Get()->DrawIndexedInstanced(
                    full_screen_triangle_info_.index_count, 1, 0, 0, 0);

                // Set render target to pixel shader resource barriers
                for (uint32_t i = 0; i < (uint32_t)lighting_pass::RenderTargetIndex::COUNT; ++i)
                {
                    dx12_util::Barrier rt_to_pr_barrier(
                        render_target_textures[i]->Get(), command_list.Get(),
                        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                    // Set current state to pixel shader resource
                    render_target_textures[i]->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                }
            });

            // Reset buffer handles
            full_screen_triangle_info_ = FullScreenTriangleInfo();
            render_target_texture_handles_ = nullptr;
            inv_view_proj_matrix_buffer_handle_ = nullptr;
            right_config_buffer_handle_ = nullptr;
            rights_buffer_handle_ = nullptr;

            // Clear current tokens
            current_read_token_ = ResourceAccessToken();
            current_write_token_ = ResourceAccessToken();

            return true; // Execution successful
        }
    );
}

void LightingPass::SetFullScreenTriangleInfo(FullScreenTriangleInfo&& fs_triangle_info)
{
    assert(IsSetup() && "Instance is not setup");
    full_screen_triangle_info_ = std::move(fs_triangle_info);
}

void LightingPass::SetViewport(const D3D12_VIEWPORT& viewport)
{
    assert(IsSetup() && "Instance is not setup");
    viewport_ = viewport;
    has_viewport_ = true;
}

void LightingPass::SetScissorRect(const D3D12_RECT& scissor_rect)
{
    assert(IsSetup() && "Instance is not setup");
    scissor_rect_ = scissor_rect;
    has_scissor_rect_ = true;
}

void LightingPass::SetRenderTargetTextureHandles(const ResourceHandles* buffer_handles)
{
    assert(IsSetup() && "Instance is not setup");
    render_target_texture_handles_ = buffer_handles;
}

void LightingPass::SetInvViewProjMatrixBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    inv_view_proj_matrix_buffer_handle_ = buffer_handle;
}

void LightingPass::SetLightConfigBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    right_config_buffer_handle_ = buffer_handle;
}

void LightingPass::SetLightsBufferHandle(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    rights_buffer_handle_ = buffer_handle;
}

void LightingPass::SetAlbedoTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    albedo_texture_handle_ = texture_handle;
}

void LightingPass::SetNormalTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    normal_texture_handle_ = texture_handle;
}

void LightingPass::SetMetalnessTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    metalness_texture_handle_ = texture_handle;
}

void LightingPass::SetRoughnessTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    roughness_texture_handle_ = texture_handle;
}

void LightingPass::SetSpecularTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    specular_texture_handle_ = texture_handle;
}

void LightingPass::SetAOTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    ao_texture_handle_ = texture_handle;
}

void LightingPass::SetEmissionTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    emission_texture_handle_ = texture_handle;
}

void LightingPass::SetMaskMaterialTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    mask_material_texture_handle_ = texture_handle;
}

void LightingPass::SetMaskShadowTextureHandle(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    mask_shadow_texture_handle_ = texture_handle;
}

void LightingPass::SetDepthStencilTextureHandle(const ResourceHandle* depth_stencil_texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    depth_stencil_texture_handle_ = depth_stencil_texture_handle;
}

const ResourceHandle* LightingPass::GetInvViewProjMatrixBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return inv_view_proj_matrix_buffer_handle_;
}

const ResourceHandle* LightingPass::GetLightConfigBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return right_config_buffer_handle_;
}

const ResourceHandle* LightingPass::GetLightsBufferHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return rights_buffer_handle_;
}

const ResourceHandle* LightingPass::GetAlbedoTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return albedo_texture_handle_;
}

const ResourceHandle* LightingPass::GetNormalTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return normal_texture_handle_;
}

const ResourceHandle* LightingPass::GetMetalnessTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return metalness_texture_handle_;
}

const ResourceHandle* LightingPass::GetRoughnessTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return roughness_texture_handle_;
}

const ResourceHandle* LightingPass::GetSpecularTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return specular_texture_handle_;
}

const ResourceHandle* LightingPass::GetAOTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return ao_texture_handle_;
}

const ResourceHandle* LightingPass::GetEmissionTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return emission_texture_handle_;
}

const ResourceHandle* LightingPass::GetMaskMaterialTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return mask_material_texture_handle_;
}

const ResourceHandle* LightingPass::GetMaskShadowTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return mask_shadow_texture_handle_;
}

const ResourceHandle* LightingPass::GetDepthStencilTextureHandle() const
{
    assert(IsSetup() && "Instance is not setup");
    return depth_stencil_texture_handle_;
}

const ResourceAccessToken& LightingPass::GetReadAccessToken() const
{
    assert(IsSetup() && "Instance is not setup");
    return current_read_token_;
}

const ResourceAccessToken& LightingPass::GetWriteAccessToken() const
{
    assert(IsSetup() && "Instance is not setup");
    return current_write_token_;
}

} // namespace render_graph