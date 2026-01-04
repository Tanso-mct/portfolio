#pragma once

#include <DirectXMath.h>

#include "render_graph/include/resource_handle.h"
#include "render_graph/include/command_handle.h"
#include "render_graph/include/material_handle.h"
#include "render_graph/include/material_manager.h"
#include "render_graph/include/light_handle.h"

#include "render_graph/include/imgui_context_wrapper.h"
#include "render_graph/include/light.h"
#include "render_graph/include/material.h"

#include "render_graph/include/buffer_upload_pass.h"
#include "render_graph/include/geometry_pass.h"
#include "render_graph/include/imgui_pass.h"
#include "render_graph/include/shadow_composition_pass.h"

#include "mono_service/include/service_command.h"
#include "mono_graphics_service/include/dll_config.h"
#include "mono_graphics_service/include/graphics_service.h"

namespace mono_graphics_service
{

// The graphics command list class
class MONO_GRAPHICS_SERVICE_DLL GraphicsCommandList :
    public mono_service::ServiceCommandList
{
public:
    GraphicsCommandList() = default;
    ~GraphicsCommandList() override = default;

    // Get the target swap chain handle
    const render_graph::ResourceHandle* GetSwapChainHandle() const;

    // Set the target swap chain handle
    void SetSwapChainHandle(const render_graph::ResourceHandle* swap_chain_handle);

    // Check if the target handle is set
    bool HasSwapChainHandle() const;

    // Get the command set handles
    const render_graph::CommandSetHandle* GetCommandSetHandle() const;

    // Set the command set handles
    void SetCommandSetHandle(const render_graph::CommandSetHandle* command_set_handle);

    // Check if the command set handle is set
    bool HasCommandSetHandle() const;

    // Create a swap chain and output its handle
    void CreateSwapChain(
        render_graph::ResourceHandle* out_swap_chain_handle, 
        HWND hwnd, UINT width, UINT height, UINT buffer_count, UINT sync_interval = 1, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create a command set and output its handle
    void CreateCommandSet(
        render_graph::CommandSetHandles* out_command_set_handles, 
        UINT count, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);

    // Create vertex buffer and output its handle
    void CreateVertexBuffer(
        render_graph::ResourceHandle* out_handle, 
        size_t vertex_count, const render_graph::geometry_pass::Vertex* initial_data, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create index buffer and output its handle
    void CreateIndexBuffer(
        render_graph::ResourceHandle* out_handle, 
        size_t index_count, const render_graph::geometry_pass::Index* initial_data, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create matrix buffer and output its handle
    void CreateMatrixBuffer(render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix = L"Unknown");

    // Create shader resource texture2D and output its handle
    void CreateShaderResourceTexture2D(
        render_graph::ResourceHandle* out_handle, render_graph::ResourceHandle* out_upload_handle,
        UINT width, UINT height, DXGI_FORMAT format, const void* data = nullptr, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create world buffer for geometry pass
    void CreateWorldBufferForGeometryPass(render_graph::ResourceHandle* out_handle, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create G-Buffer textures for geometry pass
    void CreateTexture2DsForGeometryPass(
        std::vector<render_graph::ResourceHandles>* out_gbuffer_handles,
        UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix = L"Unknown");

    // Create depth-stencil buffer for geometry pass
    void CreateDepthStencilForGeometryPass(
        render_graph::ResourceHandles* out_depth_stencil_handles, 
        UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix = L"Unknown");

    // Create full-screen triangle vertex buffer for composition pass
    void CreateFullScreenTriangleVertexBufferForComposition(
        render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix = L"Unknown");

    // Create full-screen triangle index buffer for composition pass
    void CreateFullScreenTriangleIndexBufferForComposition(
        render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix = L"Unknown");

    // Create ImGui render target textures
    void CreateImguiRenderTargetTextures(
        render_graph::ResourceHandles* out_handles, UINT client_width, UINT client_height, UINT back_buffer_count,
        std::wstring debug_name_prefix = L"Unknown");

    // Create Imgui context, its not Dear ImGui context but wrapper class
    void CreateImguiContext(
        render_graph::ImguiContextHandle* out_handle, HWND hwnd,
        render_graph::ImguiContext::ContextCreateFunc&& create_func, 
        render_graph::ImguiContext::ContextDestroyFunc&& destroy_func);

    // Create lights buffer and its upload buffer
    void CreateLightsBuffer(
        std::vector<render_graph::ResourceHandle>* out_handles, 
        std::vector<render_graph::ResourceHandle>* out_upload_handles,
        size_t back_buffer_count, size_t max_lights, std::wstring debug_name_prefix = L"Unknown");

    // Create light config buffer
    void CreateLightConfigBuffer(render_graph::ResourceHandle* out_handle, 
        std::unique_ptr<render_graph::Light::LightConfigBuffer> initial_data = nullptr, 
        std::wstring debug_name_prefix = L"Unknown");

    // Create lighting pass render target textures
    void CreateRenderTargetTexturesForLightingPasse(
        std::vector<render_graph::ResourceHandles>* out_handles,
        UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix = L"Unknown");

    // Create shadow composition config buffer
    void CreateShadowCompositionConfigBuffer(
        render_graph::ResourceHandle* out_handle,
        std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> initial_data = nullptr,
        std::wstring debug_name_prefix = L"Unknown");

    // Create shadow composition pass render target textures
    void CreateRenderTargetTexturesForShadowCompositionPass(
        std::vector<render_graph::ResourceHandles>* out_handles,
        UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix = L"Unknown");

    // Create material and output its handle
    template <typename MaterialType>
    void CreateMaterial(
        render_graph::MaterialHandle* out_handle,
        std::unique_ptr<render_graph::Material::SetupParam> setup_param)
    {
        AddCommand([out_handle, param = std::move(setup_param)](mono_service::ServiceAPI& api) mutable -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
                "GraphicsServiceAPI must be derived from ServiceAPI.");
            GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

            // Create instance of MaterialType
            std::unique_ptr<render_graph::Material> material
                = std::make_unique<MaterialType>(
                    graphics_service_api.GetResourceAdder(), graphics_service_api.GetResourceEraser());
            if (!material)
                return false; // Failure

            // Setup material
            if (!material->Setup(*param))
                return false; // Failure

            // Add material to resource manager
            *out_handle = graphics_service_api.GetMaterialAdder().AddMaterial(std::move(material));
            if (!out_handle->IsValid())
                return false; // Failure

            // Get buffer update pass
            render_graph::RenderPassBase& pass 
                = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
            render_graph::BufferUploadPass* buffer_update_pass 
                = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
            assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

            bool success = true;
            render_graph::MaterialManager::GetInstance().WithLock([&](render_graph::MaterialManager& material_manager)
            {
                // Get material
                render_graph::Material& material = material_manager.GetMaterial(out_handle);

                // Get material buffer data
                uint32_t data_size = 0;
                const void* material_data = material.GetBufferData(data_size);

                // Add buffer update task
                render_graph::BufferUploadPass::UploadTask task;
                task.buffer_handle = material.GetBufferHandle();
                task.data = material_data;
                task.size = data_size;
                if (!buffer_update_pass->AddUploadTask(std::move(task)))
                {
                    success = false;
                    return; // Failure
                }
            });

            return success;
        });
    }

    // Create directional light and output its handle
    void CreateDirectionalLight(
        render_graph::LightHandle* out_handle,
        std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t back_buffer_count,
        std::wstring debug_name_prefix = L"Unknown");

    // Create ambient light and output its handle
    void CreateAmbientLight(
        render_graph::LightHandle* out_handle,
        std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t back_buffer_count);

    // Create point light and output its handle
    void CreatePointLight(
        render_graph::LightHandle* out_handle,
        std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t back_buffer_count);

    // Upload light using its handle
    void UploadLight(const render_graph::LightHandle* light_handle);

    // Add light upload pass to render graph
    void AddLightUploadPassToGraph(
        const render_graph::ResourceHandle* lights_upload_buffer_handle,
        const render_graph::ResourceHandle* light_config_buffer_handle,
        render_graph::Light::LightConfigBuffer light_config);

    // Update buffer data in render graph
    void UpdateMatrixBuffer(
        const render_graph::ResourceHandle* buffer_handle, std::unique_ptr<DirectX::XMMATRIX> matrix_data);

    // Update world buffer data for geometry pass in render graph
    void UpdateWorldBufferForGeometryPass(
        const render_graph::ResourceHandle* buffer_handle, 
        std::unique_ptr<render_graph::geometry_pass::WorldBuffer> world_buffer_data);

    // Update material data in render graph
    void UpdateMaterialBuffer(
        const render_graph::MaterialHandle* material_handle,
        std::unique_ptr<render_graph::Material::SetupParam> material_setup_param);

    // Update light data in render graph
    void UpdateLightBuffer(
        const render_graph::LightHandle* light_handle,
        std::unique_ptr<render_graph::Light::SetupParam> light_setup_param);

    // Update light view-projection matrix buffer data in render graph
    void UpdateLightViewProjMatrixBuffer(
        const render_graph::LightHandle* light_handle,
        std::unique_ptr<DirectX::XMFLOAT3> light_position, std::unique_ptr<DirectX::XMFLOAT3> light_rotation);

    // Update lights buffer data in render graph
    void UpdateLightsBuffer(
        const render_graph::ResourceHandle* lights_buffer_handle, 
        const render_graph::ResourceHandle* lights_upload_buffer_handle, size_t light_max_count);

    // Update shadow composition config buffer data in render graph
    void UpdateShadowCompositionConfigBuffer(
        const render_graph::ResourceHandle* buffer_handle,
        std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> config_data);

    // Add buffer update pass to render graph
    void AddBufferUploadPassToGraph();

    // Update shader resource texture2D data in render graph
    void UpdateShaderResourceTexture2D(
        const render_graph::ResourceHandle* texture_handle,
        const render_graph::ResourceHandle* upload_handle, const void* data);

    // Add texture upload pass to render graph
    void AddTextureUploadPassToGraph();

    // Add light which casts shadow to render graph
    void CastShadow(const render_graph::LightHandle* light_handle);

    // Add draw shadow caster mesh command to geometry pass
    void DrawShadowCasterMesh(
        const render_graph::ResourceHandle* world_matrix_buffer_handle,
        const render_graph::ResourceHandle* vertex_buffer_handle,
        const render_graph::ResourceHandle* index_buffer_handle, uint32_t index_count);

    // Add shadowing pass to render graph
    void AddShadowingPassToGraph(UINT frame_index);

    // Add draw mesh command to geometry pass
    void DrawMesh(
        const render_graph::ResourceHandle* world_matrix_buffer_handle,
        const render_graph::MaterialHandle* material_handle,
        const render_graph::ResourceHandle* vertex_buffer_handle,
        const render_graph::ResourceHandle* index_buffer_handle, uint32_t index_count);

    // Add geometry pass to render graph
    void AddGeometryPassToGraph(
        const render_graph::ResourceHandles* gbuffer_handles,
        const render_graph::ResourceHandle* depth_stencil_handle,
        const render_graph::ResourceHandle* view_proj_matrix_buffer_handle,
        D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect);

    // Add shadow composition pass to render graph
    void AddShadowCompositionPassToGraph(
        const render_graph::ResourceHandles* render_target_handles,
        const render_graph::ResourceHandle* composition_config_buffer_handle,
        const render_graph::ResourceHandle* camera_inv_view_proj_matrix_buffer_handle,
        const render_graph::ResourceHandle* gbuffer_depth_stencil_handle,
        const render_graph::ResourceHandles* gbuffer_render_target_handles,
        const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle,
        const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle,
        D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect);

    // Add lighting pass to render graph
    void AddLightingPassToGraph(
        const render_graph::ResourceHandles* render_target_handles,
        const render_graph::ResourceHandle* camera_inv_view_proj_matrix_buffer_handle,
        const render_graph::ResourceHandle* light_config_buffer_handle,
        const render_graph::ResourceHandle* lights_buffer_handle,
        const render_graph::ResourceHandle* gbuffer_depth_stencil_handle,
        const render_graph::ResourceHandles* gbuffer_render_target_handles,
        const render_graph::ResourceHandles* shadow_composition_render_target_handles,
        const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle,
        const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle,
        D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect);

    // Add ImGui pass to render graph
    void AddImguiPassToGraph(
        const render_graph::ResourceHandle* target_texture_handle,
        render_graph::ImguiPass::DrawFunc draw_func, const render_graph::ImguiContextHandle* context_handle);

    // Add composition pass to render graph
    void AddCompositionPassToGraph(
        const render_graph::ResourceHandle* swap_chain_handle, DirectX::XMFLOAT4 clear_color,
        const render_graph::ResourceHandle* post_process_texture_handle, 
        const render_graph::ResourceHandle* ui_texture_handle,
        const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle,
        const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle,
        D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect);

    // Destroy resource in render graph
    void DestroyResource(const render_graph::ResourceHandle* resource_handle);

    // Destroy command set in render graph
    void DestroyCommandSet(const render_graph::CommandSetHandle* command_set_handle);

    // Destroy ImGui context in render graph
    void DestroyImguiContext(const render_graph::ImguiContextHandle* context_handle);

    // Destroy material in render graph
    void DestroyMaterial(const render_graph::MaterialHandle* material_handle);

    // Destroy light in render graph
    void DestroyLight(const render_graph::LightHandle* light_handle);

protected:
    // The target swap chain handle
    const render_graph::ResourceHandle* swap_chain_handle_ = nullptr;

    // The command set handles
    const render_graph::CommandSetHandle* command_set_handle_ = nullptr;
};

} // namespace mono_graphics_service