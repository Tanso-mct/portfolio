#pragma once
#include "mono_d3d12/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <d3d12.h>

#include "render_graph/include/resource_handle.h"
#include "render_graph/include/command_handle.h"
#include "render_graph/include/imgui_context_handle.h"
#include "render_graph/include/imgui_context_wrapper.h"
#include "mono_service/include/service.h"

namespace mono_d3d12
{
    constexpr size_t WindowRenderBindComponentMaxCount = 5;
    class MONO_D3D12_API WindowRenderBindComponent
    {
    private:
        // The service proxy to the graphics service
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

        // The swap chain handle associated with the window
        render_graph::ResourceHandle swap_chain_handle_ = render_graph::ResourceHandle();

        // The command set handles for rendering
        render_graph::CommandSetHandles command_set_handles_ = render_graph::CommandSetHandles();

        // The G-Buffer texture handles for rendering
        std::vector<render_graph::ResourceHandles> gbuffer_texture_handles_ = {};

        // The depth stencil texture for geometry pass
        std::vector<render_graph::ResourceHandle> gdepth_stencil_texture_handles_ = {};

        // The full-screen triangle vertex buffer handle for composition pass
        render_graph::ResourceHandle full_screen_triangle_vertex_buffer_handle_ = render_graph::ResourceHandle();

        // The full-screen triangle index buffer handle for composition pass
        render_graph::ResourceHandle full_screen_triangle_index_buffer_handle_ = render_graph::ResourceHandle();

        // Imgui render target textures
        render_graph::ResourceHandles imgui_render_target_texture_handles_ = render_graph::ResourceHandles();

        // Imgui context
        render_graph::ImguiContextHandle imgui_context_handle_ = render_graph::ImguiContextHandle();

        // Lights buffer handles
        std::vector<render_graph::ResourceHandle> lights_buffer_handles_ = {};

        // Lights buffer upload handles
        std::vector<render_graph::ResourceHandle> lights_upload_buffer_handles_ = {};

        // Light configuration buffer handle
        render_graph::ResourceHandle light_config_buffer_handle_ = render_graph::ResourceHandle();

        // Render target texture handles for lighting passes
        std::vector<render_graph::ResourceHandles> lighting_render_target_texture_handles_ = {};

        // Shadow composition config buffer handle
        render_graph::ResourceHandle shadow_composition_config_buffer_handle_ = render_graph::ResourceHandle();

        // Render target texture handles for shadow composition pass
        std::vector<render_graph::ResourceHandles> shadow_composition_render_target_texture_handles_ = {};

        // Imgui context creation and destruction functions
        render_graph::ImguiContext::ContextCreateFunc imgui_create_func_ = nullptr;

        // Imgui context destruction function
        render_graph::ImguiContext::ContextDestroyFunc imgui_destroy_func_ = nullptr;

        // The viewport for this window
        D3D12_VIEWPORT view_port_ = {};

        // The scissor rect for this window
        D3D12_RECT scissor_rect_ = {};

    public:
        WindowRenderBindComponent();
        ~WindowRenderBindComponent();

        struct SetupParam
        {
            // The Imgui context creation function
            render_graph::ImguiContext::ContextCreateFunc imgui_create_func = nullptr;

            // The Imgui context destruction function
            render_graph::ImguiContext::ContextDestroyFunc imgui_destroy_func = nullptr;
        };
        void Setup(SetupParam &param);

        // Create the resources for rendering
        // You need to call this method after the window is created
        bool CreateBindResources(
            HWND hwnd, uint32_t client_width, uint32_t client_height, uint32_t buffer_count, uint32_t max_lights);

        // Check if the bind resources are created
        bool IsBindResourcesCreated() const { return swap_chain_handle_.IsValid(); }

        // Get the swap chain handle associated with the window
        const render_graph::ResourceHandle* GetSwapChainHandle() const;

        // Get the command set handles for rendering
        const render_graph::CommandSetHandles* GetCommandSetHandles() const;

        // Get the G-Buffer texture handles for rendering
        const std::vector<render_graph::ResourceHandles>* GetGBufferTextureHandles() const;

        // Get the depth stencil texture handle for geometry pass
        const std::vector<render_graph::ResourceHandle>* GetGDepthStencilTextureHandles() const;

        // Get the full-screen triangle vertex buffer handle for composition pass
        const render_graph::ResourceHandle* GetFullScreenTriangleVertexBufferHandle() const;

        // Get the full-screen triangle index buffer handle for composition pass
        const render_graph::ResourceHandle* GetFullScreenTriangleIndexBufferHandle() const;

        // Get the Imgui render target texture handles
        const std::vector<render_graph::ResourceHandle>* GetImguiRenderTargetTextureHandles() const;

        // Get the Imgui context handle
        const render_graph::ImguiContextHandle* GetImguiContextHandle() const;

        // Get the lights buffer handles
        const std::vector<render_graph::ResourceHandle>* GetLightsBufferHandles() const;

        // Get the lights buffer upload handles
        const std::vector<render_graph::ResourceHandle>* GetLightsUploadBufferHandles() const;

        // Get the light configuration buffer handle
        const render_graph::ResourceHandle* GetLightConfigBufferHandle() const;

        // Get the render target texture handles for lighting passes
        const std::vector<render_graph::ResourceHandles>* GetLightingRenderTargetTextureHandles() const;

        // Get the shadow composition config buffer handle
        const render_graph::ResourceHandle* GetShadowCompositionConfigBufferHandle() const;

        // Get the render target texture handles for shadow composition pass
        const std::vector<render_graph::ResourceHandles>* GetShadowCompositionRenderTargetTextureHandles() const;

        // Get the viewport for this window
        const D3D12_VIEWPORT& GetViewPort() const { return view_port_; }

        // Get the scissor rect for this window
        const D3D12_RECT& GetScissorRect() const { return scissor_rect_; }
    };

    extern MONO_D3D12_API riaecs::ComponentRegistrar
    <WindowRenderBindComponent, WindowRenderBindComponentMaxCount> WindowRenderBindComponentID;

} // namespace mono_d3d12