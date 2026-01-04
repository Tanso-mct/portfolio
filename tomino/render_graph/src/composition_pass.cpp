#include "render_graph/src/pch.h"
#include "render_graph/include/composition_pass.h"

using Microsoft::WRL::ComPtr;

#include "utility_header/logger.h"
#include "directx12_util/include/wrapper.h"
#include "directx12_util/include/d3dx12.h"
#include "directx12_util/include/helper.h"
#include "render_graph/include/resource_manager.h"
#include "render_graph/include/heap_manager.h"

namespace render_graph
{

using namespace composition_pass;

CompositionPass::CompositionPass()
{
}

CompositionPass::~CompositionPass()
{
}

bool CompositionPass::Setup(std::unique_ptr<Pipeline> composition_pipeline)
{
    composition_pipeline_ = std::move(composition_pipeline);
    assert(composition_pipeline_ != nullptr && "Composition pipeline must not be null");
    if (!composition_pipeline_->Setup())
        return false;
    
    return true; // Setup successful
}

bool CompositionPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance must be setup before use.");

    return render_graph.AddPass
    (
        // Render pass handle ID
        CompositionPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            assert(swap_chain_handle_->IsValid() && "Swap chain handle is not set.");
            assert(post_process_texture_handle_->IsValid() && "Post-process texture handle is not set.");
            assert(ui_texture_handle_->IsValid() && "UI texture handle is not set.");
            assert(full_screen_triangle_info_.vertex_buffer_handle->IsValid() && "Full-screen triangle vertex buffer handle is not set.");
            assert(full_screen_triangle_info_.index_buffer_handle->IsValid() && "Full-screen triangle index buffer handle is not set.");

            builder.Write(swap_chain_handle_); // Declare write access to the swap chain

            builder.Read(post_process_texture_handle_); // Declare read access to the post-process texture if set
            builder.Read(ui_texture_handle_); // Declare read access to the UI texture if set
            builder.Read(full_screen_triangle_info_.vertex_buffer_handle); // Declare read access to full-screen triangle vertex buffer
            builder.Read(full_screen_triangle_info_.index_buffer_handle); // Declare read access to full-screen triangle index buffer
            
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
            composition_pipeline_->SetPipeline(command_list.Get());

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
                // Get swap chain for writing
                dx12_util::SwapChain* swap_chain
                    = dynamic_cast<dx12_util::SwapChain*>(&manager.GetWriteResource(swap_chain_handle_, write_token));
                assert(swap_chain != nullptr); // Ensure the cast succeeded

                // Set present to render target barrier
                dx12_util::Barrier barrier_plensent_to_rt(
                    swap_chain->GetCurrentBackBuffer(), command_list.Get(), 
                    D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

                // Get render target view
                D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = swap_chain->GetCurrentBackBufferView();

                // Set render target
                command_list.Get()->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

                // Clear render target
                command_list.Get()->ClearRenderTargetView(rtv_handle, clear_color_, 0, nullptr);

                // Set root parameters
                composition_pipeline_->SetRootParameters(command_list.Get(), GetPassAPI());

                // Get vertex buffer
                const dx12_util::Resource& vertex_buffer_resource
                    = manager.GetReadResource(full_screen_triangle_info_.vertex_buffer_handle, read_token);
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
                    = manager.GetReadResource(full_screen_triangle_info_.index_buffer_handle, read_token);
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
                command_list.Get()->DrawIndexedInstanced(
                    full_screen_triangle_info_.index_count, 1, 0, 0, 0);

                // Set render target to present barrier
                dx12_util::Barrier barrier_rt_to_present(
                    swap_chain->GetCurrentBackBuffer(), command_list.Get(), 
                    D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
            });

            // Reset
            swap_chain_handle_ = nullptr;
            clear_color_[0] = 0.0f;
            clear_color_[1] = 0.0f;
            clear_color_[2] = 0.0f;
            clear_color_[3] = 1.0f;
            post_process_texture_handle_ = nullptr;
            ui_texture_handle_ = nullptr;
            full_screen_triangle_info_ = FullScreenTriangleInfo();
            has_viewport_ = false;
            has_scissor_rect_ = false;

            return true; // Execution successful
        });
}

const ResourceHandle* CompositionPass::GetPostProcessTextureHandle() const
{
    assert(IsSetup() && "Instance must be setup before use.");
    return post_process_texture_handle_;
}

const ResourceHandle* CompositionPass::GetUITextureHandle() const
{
    assert(IsSetup() && "Instance must be setup before use.");
    return ui_texture_handle_;
}

const ResourceAccessToken& CompositionPass::GetWriteToken() const
{
    assert(IsSetup() && "Instance must be setup before use.");
    return current_write_token_;
}

const ResourceAccessToken& CompositionPass::GetReadToken() const
{
    assert(IsSetup() && "Instance must be setup before use.");
    return current_read_token_;
}

void CompositionPass::SetTargetSwapChain(const ResourceHandle* swap_chain_handle, const float clear_color[4])
{
    assert(IsSetup() && "Instance must be setup before use.");
    assert(swap_chain_handle->IsValid() && "Swap chain handle must be valid.");

    swap_chain_handle_ = swap_chain_handle;
    std::memcpy(clear_color_, clear_color, sizeof(float) * 4);
}

void CompositionPass::SetPostProcessTexture(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance must be setup before use.");
    post_process_texture_handle_ = texture_handle;
}

void CompositionPass::SetUITexture(const ResourceHandle* texture_handle)
{
    assert(IsSetup() && "Instance must be setup before use.");
    ui_texture_handle_ = texture_handle;
}

void CompositionPass::SetFullScreenTriangleInfo(FullScreenTriangleInfo&& info)
{
    assert(IsSetup() && "Instance must be setup before use.");
    full_screen_triangle_info_ = std::move(info);
}

void CompositionPass::SetViewport(const D3D12_VIEWPORT& viewport)
{
    assert(IsSetup() && "Instance must be setup before use.");
    viewport_ = viewport;
    has_viewport_ = true;
}

void CompositionPass::SetScissorRect(const D3D12_RECT& scissor_rect)
{
    assert(IsSetup() && "Instance must be setup before use.");
    scissor_rect_ = scissor_rect;
    has_scissor_rect_ = true;
}

} // namespace render_graph