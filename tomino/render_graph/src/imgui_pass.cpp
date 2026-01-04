#include "render_graph/src/pch.h"
#include "render_graph/include/imgui_pass.h"

#include "imgui_internal.h" // DockBuilder API
#include "imgui_impl_win32.h"
#include "imgui_impl_dx12.h"
#include "directx12_util/include/wrapper.h"
#include "render_graph/include/heap_manager.h"
#include "render_graph/include/imgui_context_manager.h"

namespace render_graph
{

using namespace imgui_pass;

ImguiPass::ImguiPass()
{
}

ImguiPass::~ImguiPass()
{
}

bool ImguiPass::Setup()
{
    // No special setup needed
    return true;
}

bool ImguiPass::AddToGraph(RenderGraph &render_graph)
{
    assert(IsSetup() && "Instance is not setup");

    return render_graph.AddPass
    (
        // Render pass handle ID
        ImguiPassHandle::ID(),

        // Setup function
        [&](RenderPassBuilder& builder) 
        {
            assert(target_texture_handle_->IsValid() && "Target texture handle is not set");

            builder.Write(target_texture_handle_); // Declare write access to target texture
            return true; // Setup successful
        },

        // Execute function
        [&](RenderPass& self_pass, RenderPassContext& context) 
        {
            assert(target_texture_handle_->IsValid() && "Target texture handle is not set");
            assert(context_handle_->IsValid() && "ImGui context handle is not set");
            assert(draw_func_ != nullptr && "Draw function is not set");

            // Get write access token for target texture
            const ResourceAccessToken& write_token = self_pass.GetWriteToken();

            // Get command list
            dx12_util::CommandList& command_list = context.GetCommandList();

            // Get Imgui context manager
            ImguiContextManager& context_manager = ImguiContextManager::GetInstance();

            context_manager.WithLock([&](ImguiContextManager& manager)
            {
                // Get ImGui context
                ImguiContext& imgui_context = manager.GetContext(context_handle_);

                // Set current ImGui context
                ImGui::SetCurrentContext(imgui_context.Get());

                // Start new frame for backends
                ImGui_ImplDX12_NewFrame();
                ImGui_ImplWin32_NewFrame();
                ImGui::NewFrame();

                // Draw custom ImGui content if draw function is set
                if (!draw_func_(imgui_context.Get()))
                    return false; // Stop execution if draw function returns false

                // Rendering
                ImGui::Render();
            });

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
                // Get target texture
                dx12_util::Resource& target_texture_resource =
                    manager.GetWriteResource(target_texture_handle_, write_token);
                dx12_util::Texture2D& target_texture =
                    static_cast<dx12_util::Texture2D&>(target_texture_resource);

                // Set barrier to transition target texture to render target state
                if (target_texture.GetCurrentState() != D3D12_RESOURCE_STATE_RENDER_TARGET)
                {
                    dx12_util::Barrier rt_barrier(
                        target_texture.Get(), command_list.Get(),
                        target_texture.GetCurrentState(), D3D12_RESOURCE_STATE_RENDER_TARGET);

                    // Update current state
                    target_texture.SetCurrentState(D3D12_RESOURCE_STATE_RENDER_TARGET);
                }

                // Get RTV CPU descriptor handle
                D3D12_CPU_DESCRIPTOR_HANDLE rtv_handle = target_texture.GetRtvCpuHandle();

                // Setup render target
                command_list.Get()->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

                // Clear render target
                float clear_color[4] =
                {
                    TARGET_CLEAR_COLOR[0],
                    TARGET_CLEAR_COLOR[1],
                    TARGET_CLEAR_COLOR[2],
                    TARGET_CLEAR_COLOR[3]
                };

                command_list.Get()->ClearRenderTargetView(rtv_handle, clear_color, 0, nullptr);

                // Record Dear ImGui draw data
                ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list.Get());

                // Set barrier to transition target texture to shader resource state
                dx12_util::Barrier srv_barrier(
                    target_texture.Get(), command_list.Get(),
                    D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
                
                // Update current state
                target_texture.SetCurrentState(D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            });

            // Reset
            target_texture_handle_ = nullptr;
            context_handle_ = nullptr;
            draw_func_ = nullptr;

            return true; // Execution successful
        }
    );
}

void ImguiPass::SetTargetTexture(const ResourceHandle* target_texture_handle)
{
    assert(IsSetup() && "Instance is not setup");
    target_texture_handle_ = target_texture_handle;
}

void ImguiPass::SetDrawFunc(DrawFunc draw_func)
{
    assert(IsSetup() && "Instance is not setup");
    draw_func_ = std::move(draw_func);
}

void ImguiPass::SetImguiContext(const ImguiContextHandle* context_handle)
{
    assert(IsSetup() && "Instance is not setup");
    context_handle_ = context_handle;
}

} // namespace render_graph