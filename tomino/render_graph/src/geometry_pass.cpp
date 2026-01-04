#include "render_graph/src/pch.h"
#include "render_graph/include/geometry_pass.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"

#include "render_graph/include/heap_manager.h"
#include "render_graph/include/material_manager.h"

namespace render_graph
{

using namespace geometry_pass;

GeometryPass::GeometryPass()
{
}

GeometryPass::~GeometryPass()
{
}

bool GeometryPass::Setup(PipelineMap&& pipelines)
{   
    assert(!pipelines.empty() && "Pipelines map is empty.");

    // Store pipelines
    pipeline_map_ = std::move(pipelines);

    // Setup pipelines
    for (auto& [material_type_handle_id, pipeline] : pipeline_map_)
        if (!pipeline->Setup())
            return false; // Pipeline setup failed

    return true; // Setup successful
}

bool GeometryPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        GeometryPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                assert(gbuffer_texture_handles_->at(i).IsValid() && "G-buffer texture handle is not set.");
            
            assert(view_proj_matrix_buffer_handle_->IsValid() && "View-projection matrix buffer handle is not set.");
            assert(depth_stencil_texture_handle_->IsValid() && "Depth texture handle is not set.");

            // G-buffer textures
            for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                builder.Write(&gbuffer_texture_handles_->at(i)); // Declare G-buffer texture writes
            
            // Depth texture
            builder.Write(depth_stencil_texture_handle_);  // Declare depth texture write

            // Mesh buffers
            for (const MeshInfo& mesh_info : mesh_infos_)
            {
                builder.Read(mesh_info.vertex_buffer_handle); // Declare vertex buffer read
                builder.Read(mesh_info.index_buffer_handle);  // Declare index buffer read
                builder.Read(mesh_info.world_matrix_buffer_handle); // Declare world matrix buffer read

                MaterialManager& material_manager = MaterialManager::GetInstance();
                material_manager.WithLock([&](MaterialManager& manager)
                {
                    // Get material
                    const Material& material = manager.GetMaterial(mesh_info.material_handle);

                    // Declare material resources
                    material.DeclareResources(builder);
                });
            }

            // Camera buffer
            builder.Read(view_proj_matrix_buffer_handle_); // Declare view-projection matrix buffer read

            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                assert(gbuffer_texture_handles_->at(i).IsValid() && "G-buffer texture handle is not set.");
            
            assert(view_proj_matrix_buffer_handle_->IsValid() && "View-projection matrix buffer handle is not set.");
            assert(has_viewport_ && "Viewport is not set.");
            assert(has_scissor_rect_ && "Scissor rect is not set.");

            // Get write access tokens
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();
            current_write_token_ = write_token;

            // Get read access tokens
            const ResourceAccessToken& read_token = self_pass.GetReadToken();
            current_read_token_ = read_token;

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

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
                dx12_util::Texture2D* render_target_textures[(UINT)GBufferIndex::COUNT];
                for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                {
                    // Get resource
                    dx12_util::Resource& render_target_resource 
                        = manager.GetWriteResource(&gbuffer_texture_handles_->at(i), write_token);

                    // Cast to Texture2D and store
                    dx12_util::Texture2D* render_target_texture 
                        = dynamic_cast<dx12_util::Texture2D*>(&render_target_resource);
                    
                    // Store in array
                    render_target_textures[i] = render_target_texture;
                }

                // Set before to render target state barriers
                for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                {
                    if (render_target_textures[i]->GetCurrentState() == D3D12_RESOURCE_STATE_RENDER_TARGET)
                        continue; // Skip if already in render target state

                    dx12_util::Barrier rt_barrier(
                        render_target_textures[i]->Get(), command_list.Get(),
                        render_target_textures[i]->GetCurrentState(), D3D12_RESOURCE_STATE_RENDER_TARGET);

                    // Set current state to render target
                    render_target_textures[i]->SetCurrentState(D3D12_RESOURCE_STATE_RENDER_TARGET);
                }

                // Create RTV handles array
                D3D12_CPU_DESCRIPTOR_HANDLE rtv_handles[(UINT)GBufferIndex::COUNT];
                for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                    rtv_handles[i] = render_target_textures[i]->GetRtvCpuHandle();

                // Get depth stencil
                dx12_util::Resource& depth_resource = manager.GetWriteResource(depth_stencil_texture_handle_, write_token);
                dx12_util::Texture2D& depth_texture_2d = dynamic_cast<dx12_util::Texture2D&>(depth_resource);

                // Set before to depth write state barrier
                if (depth_texture_2d.GetCurrentState() != D3D12_RESOURCE_STATE_DEPTH_WRITE)
                {
                    dx12_util::Barrier depth_to_dw_barrier(
                        depth_texture_2d.Get(), command_list.Get(),
                        depth_texture_2d.GetCurrentState(), D3D12_RESOURCE_STATE_DEPTH_WRITE);

                    // Set current state to depth write
                    depth_texture_2d.SetCurrentState(D3D12_RESOURCE_STATE_DEPTH_WRITE);
                }

                // Get DSV handle
                D3D12_CPU_DESCRIPTOR_HANDLE dsv_handle = depth_texture_2d.GetDsvCpuHandle();

                // Set render targets
                command_list.Get()->OMSetRenderTargets(
                    (UINT)GBufferIndex::COUNT, rtv_handles, FALSE, &dsv_handle);

                // Clear render targets
                for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                {
                    float clear_color[4] = 
                    {
                        GBUFFER_CLEAR_COLORS[i][0],
                        GBUFFER_CLEAR_COLORS[i][1],
                        GBUFFER_CLEAR_COLORS[i][2],
                        GBUFFER_CLEAR_COLORS[i][3]
                    };

                    command_list.Get()->ClearRenderTargetView(
                        rtv_handles[i], clear_color, 0, nullptr);
                }

                // Clear depth stencil
                command_list.Get()->ClearDepthStencilView(
                    dsv_handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, &scissor_rect_);

                // Render each mesh buffer
                for (const MeshInfo& mesh_info : mesh_infos_)
                {
                    // Hold material type handle ID
                    MaterialTypeHandleID material_type_handle_id;

                    MaterialManager& material_manager = MaterialManager::GetInstance();
                    material_manager.WithLock([&](MaterialManager& manager)
                    {
                        // Get material
                        const Material& material = manager.GetMaterial(mesh_info.material_handle);

                        // Get material type handle ID
                        material_type_handle_id = material.GetMaterialTypeHandleID();
                    });

                    // Find pipeline for the material type
                    auto pipeline_iter = pipeline_map_.find(material_type_handle_id);
                    assert(
                        pipeline_iter != pipeline_map_.end() && 
                        "Pipeline for the material type not found in GeometryPass.");
                    Pipeline& pipeline = *(pipeline_iter->second);

                    // Set pipeline state and root signature
                    pipeline.SetPipeline(command_list.Get());

                    material_manager.WithLock([&](MaterialManager& manager)
                    {
                        // Set drawing world matrix buffer handle
                        drawing_world_matrix_buffer_handle_ = mesh_info.world_matrix_buffer_handle;

                        // Set drawing material handle
                        drawing_material_handle_ = mesh_info.material_handle;

                        // Set root parameters
                        pipeline.SetRootParameters(command_list.Get(), GetPassAPI());
                    });

                    // Get vertex buffer
                    const dx12_util::Resource& vertex_buffer_resource
                        = manager.GetReadResource(mesh_info.vertex_buffer_handle, read_token);
                    const dx12_util::Buffer& vertex_buffer 
                        = dynamic_cast<const dx12_util::Buffer&>(vertex_buffer_resource);

                    // Create vertex buffer view
                    D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = {};
                    vertex_buffer_view.BufferLocation = vertex_buffer.GetGPUVirtualAddress();
                    vertex_buffer_view.SizeInBytes = vertex_buffer.GetSize();
                    vertex_buffer_view.StrideInBytes = sizeof(Vertex);

                    // Set vertex buffer view
                    command_list.Get()->IASetVertexBuffers(0, 1, &vertex_buffer_view);

                    // Get index buffer
                    const dx12_util::Resource& index_buffer_resource
                        = manager.GetReadResource(mesh_info.index_buffer_handle, read_token);
                    const dx12_util::Buffer& index_buffer 
                        = dynamic_cast<const dx12_util::Buffer&>(index_buffer_resource);

                    // Create index buffer view
                    D3D12_INDEX_BUFFER_VIEW index_buffer_view = {};
                    index_buffer_view.BufferLocation = index_buffer.GetGPUVirtualAddress();
                    index_buffer_view.SizeInBytes = index_buffer.GetSize();
                    index_buffer_view.Format = INDEX_BUFFER_FORMAT;

                    // Set index buffer view
                    command_list.Get()->IASetIndexBuffer(&index_buffer_view);

                    // Draw call
                    command_list.Get()->DrawIndexedInstanced(mesh_info.index_count, 1, 0, 0, 0);
                }

                // Set render target to pixel shader resource state barriers
                for (UINT i = 0; i < (UINT)GBufferIndex::COUNT; ++i)
                {
                    dx12_util::Barrier rt_to_pr_barrier(
                        render_target_textures[i]->Get(), command_list.Get(),
                        D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                    // Set current state to pixel shader resource
                    render_target_textures[i]->SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                }

                // Set depth stencil to pixel shader resource state barrier
                dx12_util::Barrier depth_to_dr_barrier(
                    depth_texture_2d.Get(), command_list.Get(),
                    D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

                // Set current state to pixel shader resource
                depth_texture_2d.SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            });

            // Clear draw target mesh info for next frame
            mesh_infos_.clear();

            return true; // Execution successful
        }
    );
}

void render_graph::GeometryPass::SetGBuffers(const ResourceHandles* texture_handles)
{
    assert(IsSetup() && "Instance is not setup");
    gbuffer_texture_handles_ = texture_handles;
}

void GeometryPass::SetDepthStencil(const ResourceHandle* depth_stencil_texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    depth_stencil_texture_handle_ = depth_stencil_texture_handle;
}

void GeometryPass::AddDrawMeshInfo(MeshInfo&& mesh_buffer_info)
{
    assert(IsSetup() && "Instance is not setup");
    mesh_infos_.emplace_back(std::move(mesh_buffer_info));
}

void render_graph::GeometryPass::SetViewProjMatrixBuffer(const ResourceHandle* buffer_handle)
{
    assert(IsSetup() && "Instance is not setup");
    view_proj_matrix_buffer_handle_ = buffer_handle;
}

void GeometryPass::SetViewport(const D3D12_VIEWPORT& viewport)
{
    viewport_ = viewport;
    has_viewport_ = true;
}

void GeometryPass::SetScissorRect(const D3D12_RECT& scissor_rect)
{
    scissor_rect_ = scissor_rect;
    has_scissor_rect_ = true;
}

const ResourceHandle* GeometryPass::GetViewProjMatrixBufferHandle() const
{
    return view_proj_matrix_buffer_handle_;
}

const ResourceHandle* GeometryPass::GetDrawingWorldMatrixBufferHandle() const
{
    return drawing_world_matrix_buffer_handle_;
}

const MaterialHandle* GeometryPass::GetDrawingMaterialHandle() const
{
    return drawing_material_handle_;
}

const ResourceAccessToken& GeometryPass::GetCurrentReadToken() const
{
    return current_read_token_;
}

const ResourceAccessToken& GeometryPass::GetCurrentWriteToken() const
{
    return current_write_token_;
}

} // namespace render_graph