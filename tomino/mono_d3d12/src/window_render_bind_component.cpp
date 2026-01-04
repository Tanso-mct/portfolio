#include "mono_d3d12/src/pch.h"
#include "mono_d3d12/include/window_render_bind_component.h"

#include "mono_adapter/include/service_adapter.h"
#include "mono_graphics_service/include/graphics_command_list.h"

mono_d3d12::WindowRenderBindComponent::WindowRenderBindComponent()
{
    // Get singleton graphics service adapter
    mono_adapter::GraphicsServiceAdapter& graphics_service_adapter
        = mono_adapter::GraphicsServiceAdapter::GetInstance();
        
    // Create graphics service proxy
    graphics_service_proxy_ = graphics_service_adapter.GetGraphicsService().CreateServiceProxy();
    assert(graphics_service_proxy_ != nullptr);
}

mono_d3d12::WindowRenderBindComponent::~WindowRenderBindComponent()
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create GraphicsCommandList");

    // Destroy swap chain
    graphics_command_list->DestroyResource(&swap_chain_handle_);

    // Destroy command sets
    for (const auto& handle : command_set_handles_)
        graphics_command_list->DestroyCommandSet(&handle);

    // Destroy G-Buffer textures
    for (const auto& handles : gbuffer_texture_handles_)
        for (const auto& handle : handles)
            graphics_command_list->DestroyResource(&handle);

    // Destroy depth stencil textures
    for (const auto& handle : gdepth_stencil_texture_handles_)
        graphics_command_list->DestroyResource(&handle);

    // Destroy full-screen triangle vertex buffer
    graphics_command_list->DestroyResource(&full_screen_triangle_vertex_buffer_handle_);

    // Destroy full-screen triangle index buffer
    graphics_command_list->DestroyResource(&full_screen_triangle_index_buffer_handle_);

    // Destroy Imgui render target textures
    for (const auto& handle : imgui_render_target_texture_handles_)
        graphics_command_list->DestroyResource(&handle);

    // Destroy Imgui context
    graphics_command_list->DestroyImguiContext(&imgui_context_handle_);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));
}

void mono_d3d12::WindowRenderBindComponent::Setup(SetupParam &param)
{
    // Set parameters
    assert(
        param.imgui_create_func != nullptr &&
        "Imgui context create function is null");
    imgui_create_func_ = std::move(param.imgui_create_func);

    assert(
        param.imgui_destroy_func != nullptr &&
        "Imgui context destroy function is null");
    imgui_destroy_func_ = std::move(param.imgui_destroy_func);
}

bool mono_d3d12::WindowRenderBindComponent::CreateBindResources(
    HWND hwnd, uint32_t client_width, uint32_t client_height, uint32_t buffer_count, uint32_t max_lights)
{
    // Create graphics service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = graphics_service_proxy_->CreateCommandList();
    mono_graphics_service::GraphicsCommandList* graphics_command_list
        = dynamic_cast<mono_graphics_service::GraphicsCommandList*>(command_list.get());
    assert(graphics_command_list != nullptr && "Failed to create graphics command list");

    // Create swap chain in graphics service
    graphics_command_list->CreateSwapChain(
        &swap_chain_handle_, hwnd, client_width, client_height, buffer_count);

    // Create command set in graphics service
    graphics_command_list->CreateCommandSet(&command_set_handles_, buffer_count);

    // Create textures for Geometry Pass in graphics
    graphics_command_list->CreateTexture2DsForGeometryPass(
        &gbuffer_texture_handles_, client_width, client_height, buffer_count);

    // Create depth stencil texture for Geometry Pass in graphics service
    graphics_command_list->CreateDepthStencilForGeometryPass(
        &gdepth_stencil_texture_handles_, client_width, client_height, buffer_count);

    // Create full-screen triangle vertex buffer for composition pass
    graphics_command_list->CreateFullScreenTriangleVertexBufferForComposition(
        &full_screen_triangle_vertex_buffer_handle_);

    // Create full-screen triangle index buffer for composition pass
    graphics_command_list->CreateFullScreenTriangleIndexBufferForComposition(
        &full_screen_triangle_index_buffer_handle_);

    // Create Imgui render target textures in graphics service
    graphics_command_list->CreateImguiRenderTargetTextures(
        &imgui_render_target_texture_handles_, client_width, client_height, buffer_count);

    // Create Imgui context in graphics service
    graphics_command_list->CreateImguiContext(
        &imgui_context_handle_, hwnd, std::move(imgui_create_func_), std::move(imgui_destroy_func_));

    // Create lights buffer in graphics service
    graphics_command_list->CreateLightsBuffer(
        &lights_buffer_handles_, &lights_upload_buffer_handles_, buffer_count, max_lights);

    // Create light configuration buffer in graphics service
    graphics_command_list->CreateLightConfigBuffer(&light_config_buffer_handle_);

    // Create render target textures for lighting passes in graphics service
    graphics_command_list->CreateRenderTargetTexturesForLightingPasse(
        &lighting_render_target_texture_handles_, client_width, client_height, buffer_count);

    // Create shadow composition config buffer in graphics service
    graphics_command_list->CreateShadowCompositionConfigBuffer(&shadow_composition_config_buffer_handle_);

    // Create render target texture for shadow composition pass in graphics service
    graphics_command_list->CreateRenderTargetTexturesForShadowCompositionPass(
        &shadow_composition_render_target_texture_handles_, client_width, client_height, buffer_count);

    // Submit command list to graphics service
    graphics_service_proxy_->SubmitCommandList(std::move(command_list));

    // Create viewport
    view_port_.Width = static_cast<float>(client_width);
    view_port_.Height = static_cast<float>(client_height);
    view_port_.MinDepth = 0.0f;
    view_port_.MaxDepth = 1.0f;
    view_port_.TopLeftX = 0.0f;
    view_port_.TopLeftY = 0.0f;

    // Create scissor rect
    scissor_rect_.top = 0;
    scissor_rect_.bottom = static_cast<LONG>(client_height);
    scissor_rect_.left = 0;
    scissor_rect_.right = static_cast<LONG>(client_width);

    return true; // Success
}

const render_graph::ResourceHandle* mono_d3d12::WindowRenderBindComponent::GetSwapChainHandle() const
{
    assert(swap_chain_handle_.IsValid() && "Swap chain handle is not valid");
    return &swap_chain_handle_;
}

const render_graph::CommandSetHandles* mono_d3d12::WindowRenderBindComponent::GetCommandSetHandles() const
{
    for (const auto& handle : command_set_handles_)
        assert(handle.IsValid() && "Command set handle is not valid");

    return &command_set_handles_;
}

const std::vector<render_graph::ResourceHandles>* mono_d3d12::WindowRenderBindComponent::GetGBufferTextureHandles() const
{
    for (const auto& handles : gbuffer_texture_handles_)
        for (const auto& handle : handles)
            assert(handle.IsValid() && "G-Buffer texture handle is not valid");

    return &gbuffer_texture_handles_;
}

const std::vector<render_graph::ResourceHandle>* mono_d3d12::WindowRenderBindComponent::GetGDepthStencilTextureHandles() const
{
    for (const auto& handle : gdepth_stencil_texture_handles_)
        assert(handle.IsValid() && "G-Depth stencil texture handle is not valid");

    return &gdepth_stencil_texture_handles_;
}

const render_graph::ResourceHandle* mono_d3d12::WindowRenderBindComponent::GetFullScreenTriangleVertexBufferHandle() const
{
    assert(
        full_screen_triangle_vertex_buffer_handle_.IsValid() && 
        "Full-screen triangle vertex buffer handle is not valid");
    return &full_screen_triangle_vertex_buffer_handle_;
}

const render_graph::ResourceHandle* mono_d3d12::WindowRenderBindComponent::GetFullScreenTriangleIndexBufferHandle() const
{
    assert(
        full_screen_triangle_index_buffer_handle_.IsValid() && 
        "Full-screen triangle index buffer handle is not valid");
    return &full_screen_triangle_index_buffer_handle_;
}

const render_graph::ResourceHandles* mono_d3d12::WindowRenderBindComponent::GetImguiRenderTargetTextureHandles() const
{
    for (const auto& handle : imgui_render_target_texture_handles_)
        assert(handle.IsValid() && "Imgui render target texture handle is not valid");

    return &imgui_render_target_texture_handles_;
}

const render_graph::ImguiContextHandle* mono_d3d12::WindowRenderBindComponent::GetImguiContextHandle() const
{
    assert(imgui_context_handle_.IsValid() && "Imgui context handle is not valid");
    return &imgui_context_handle_;
}

const std::vector<render_graph::ResourceHandle>* mono_d3d12::WindowRenderBindComponent::GetLightsBufferHandles() const
{
    for (const auto& handle : lights_buffer_handles_)
        assert(handle.IsValid() && "Light buffer handle is not valid");

    return &lights_buffer_handles_;
}

const std::vector<render_graph::ResourceHandle>* mono_d3d12::WindowRenderBindComponent::GetLightsUploadBufferHandles() const
{
    for (const auto& handle : lights_upload_buffer_handles_)
        assert(handle.IsValid() && "Light upload buffer handle is not valid");

    return &lights_upload_buffer_handles_;
}

const render_graph::ResourceHandle* mono_d3d12::WindowRenderBindComponent::GetLightConfigBufferHandle() const
{
    assert(light_config_buffer_handle_.IsValid() && "Light configuration buffer handle is not valid");
    return &light_config_buffer_handle_;
}

const std::vector<render_graph::ResourceHandles>* mono_d3d12::WindowRenderBindComponent::GetLightingRenderTargetTextureHandles() const
{
    for (const auto& handles : lighting_render_target_texture_handles_)
        for (const auto& handle : handles)
            assert(handle.IsValid() && "Lighting render target texture handle is not valid");

    return &lighting_render_target_texture_handles_;
}

const render_graph::ResourceHandle* mono_d3d12::WindowRenderBindComponent::GetShadowCompositionConfigBufferHandle() const
{
    assert(
        shadow_composition_config_buffer_handle_.IsValid() && 
        "Shadow composition config buffer handle is not valid");
    return &shadow_composition_config_buffer_handle_;
}

const std::vector<render_graph::ResourceHandles>* mono_d3d12::WindowRenderBindComponent::GetShadowCompositionRenderTargetTextureHandles() const
{
    for (const auto& handles : shadow_composition_render_target_texture_handles_)
        for (const auto& handle : handles)
            assert(handle.IsValid() && "Shadow composition render target texture handle is not valid");

    return &shadow_composition_render_target_texture_handles_;
}

MONO_D3D12_API riaecs::ComponentRegistrar
<mono_d3d12::WindowRenderBindComponent, mono_d3d12::WindowRenderBindComponentMaxCount> mono_d3d12::WindowRenderBindComponentID;