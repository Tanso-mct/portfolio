#include "mono_graphics_service/src/pch.h"
#include "mono_graphics_service/include/graphics_command_list.h"

using namespace DirectX;

#include "directx12_util/include/helper.h"

#include "render_graph/include/resource_manager.h"
#include "render_graph/include/material_manager.h"
#include "render_graph/include/light_manager.h"

#include "render_graph/include/light_upload_pass.h"
#include "render_graph/include/buffer_upload_pass.h"
#include "render_graph/include/texture_upload_pass.h"

#include "render_graph/include/geometry_pass.h"
#include "render_graph/include/lambert_material.h"
#include "render_graph/include/phong_material.h"

#include "render_graph/include/imgui_pass.h"
#include "render_graph/include/shadow_composition_pass.h"
#include "render_graph/include/shadowing_pass.h"

#include "render_graph/include/lighting_pass.h"
#include "render_graph/include/light.h"
#include "render_graph/include/directional_light.h"
#include "render_graph/include/ambient_light.h"
#include "render_graph/include/point_light.h"

#include "render_graph/include/composition_pass.h"
#include "mono_graphics_service/include/graphics_service.h"

namespace mono_graphics_service
{

const render_graph::ResourceHandle* GraphicsCommandList::GetSwapChainHandle() const
{
    assert(swap_chain_handle_->IsValid() && "Swap chain handle is not set.");
    return swap_chain_handle_;
}

void GraphicsCommandList::SetSwapChainHandle(const render_graph::ResourceHandle* swap_chain_handle)
{
    assert(swap_chain_handle->IsValid() && "Swap chain handle must be valid.");
    swap_chain_handle_ = swap_chain_handle;
}

bool GraphicsCommandList::HasSwapChainHandle() const
{
    if (swap_chain_handle_ == nullptr)
        return false;

    return swap_chain_handle_->IsValid();
}

const render_graph::CommandSetHandle* GraphicsCommandList::GetCommandSetHandle() const
{
    assert(command_set_handle_->IsValid() && "Command set handle is not set.");
    return command_set_handle_;
}

void GraphicsCommandList::SetCommandSetHandle(
    const render_graph::CommandSetHandle* command_set_handle)
{
    assert(command_set_handle->IsValid() && "Command set handle must be valid.");
    command_set_handle_ = command_set_handle;
}

bool GraphicsCommandList::HasCommandSetHandle() const
{
    if (command_set_handle_ == nullptr)
        return false;

    return command_set_handle_->IsValid();
}

void GraphicsCommandList::CreateSwapChain(
    render_graph::ResourceHandle* out_swap_chain_handle, 
    HWND hwnd, UINT width, UINT height, UINT buffer_count, UINT sync_interval, std::wstring debug_name_prefix)
{
    AddCommand([
        hwnd, out_swap_chain_handle, 
        width, height, buffer_count, sync_interval, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create swap chain
        std::unique_ptr<dx12_util::SwapChain> swap_chain
            = dx12_util::SwapChain::CreateInstance<dx12_util::SwapChain>(
                buffer_count, sync_interval, 
                render_graph::composition_pass::SWAP_CHAIN_EFFECT, render_graph::composition_pass::SWAP_CHAIN_FORMAT, 
                debug_name_prefix + L"_SwapChain",
                dx12_util::Device::GetInstance().Get(), dx12_util::DXFactory::GetInstance().Get(),
                dx12_util::CommandQueue::GetInstance().Get(), hwnd, width, height);
        if (!swap_chain)
            return false; // Failure

        // Add swap chain to resource manager
        *out_swap_chain_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(swap_chain));
        if (!out_swap_chain_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateCommandSet(
    render_graph::CommandSetHandles* out_command_set_handles, UINT count, D3D12_COMMAND_LIST_TYPE type)
{
    AddCommand([
        out_command_set_handles, count, type](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create command sets
        for (UINT i = 0; i < count; ++i)
        {
            render_graph::CommandSetHandle command_set_handle = render_graph::CommandSetHandle();
            
            // Create command set
            std::unique_ptr<render_graph::CommandSet> command_set
                = render_graph::CommandSet::CreateInstance<render_graph::CommandSet>(type);
            if (!command_set)
                return false; // Failure

            // Add command set to command set container
            command_set_handle = graphics_service_api.GetCommandSetAdder().AddCommandSet(std::move(command_set));
            if (!command_set_handle.IsValid())
                return false; // Failure            

            // Store command set handle
            out_command_set_handles->emplace_back(std::move(command_set_handle));
        }

        return true; // Success
    });
}

void GraphicsCommandList::CreateVertexBuffer(
    render_graph::ResourceHandle* out_handle, 
    size_t vertex_count, const render_graph::geometry_pass::Vertex* initial_data, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, vertex_count, initial_data, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size = sizeof(render_graph::geometry_pass::Vertex) * vertex_count;

        // Create vertex buffer
        std::unique_ptr<dx12_util::Buffer> vertex_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_VertexBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!vertex_buffer)
            return false; // Failure

        // Initialize buffer data
        if (initial_data)
            vertex_buffer->UpdateData(initial_data, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(vertex_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateIndexBuffer(
    render_graph::ResourceHandle* out_handle, 
    size_t index_count, const render_graph::geometry_pass::Index* initial_data, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, index_count, initial_data, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size = sizeof(render_graph::geometry_pass::Index) * index_count;

        // Create index buffer
        std::unique_ptr<dx12_util::Buffer> index_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_IndexBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!index_buffer)
            return false; // Failure

        // Initialize buffer data
        if (initial_data)
            index_buffer->UpdateData(initial_data, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(index_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateMatrixBuffer(render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size = sizeof(XMMATRIX);

        // Create matrix buffer
        std::unique_ptr<dx12_util::Buffer> matrix_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_MatrixBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!matrix_buffer)
            return false; // Failure

        // Initialize buffer data
        XMMATRIX initial_data = XMMatrixIdentity();
        matrix_buffer->UpdateData(&initial_data, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(matrix_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateShaderResourceTexture2D(
    render_graph::ResourceHandle* out_handle, render_graph::ResourceHandle* out_upload_handle,
    UINT width, UINT height, DXGI_FORMAT format, const void* data, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, out_upload_handle, width, height, format, data, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Create texture2D
            std::unique_ptr<dx12_util::Texture2D> texture
                = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                    width, height, format, D3D12_HEAP_TYPE_DEFAULT,
                    D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, debug_name_prefix + L"_Texture2D",
                    dx12_util::Device::GetInstance().Get(), nullptr,
                    nullptr, nullptr, nullptr, nullptr,
                    &heap_manager.GetSrvHeapAllocator(), nullptr, nullptr);
            if (!texture)
            {
                success = false;
                return; // Failure
            }

            // Add texture to resource manager
            *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(texture));
            if (!out_handle->IsValid())
            {
                success = false;
                return; // Failure
            }
        });

        if (!success)
            return false; // Failure

        // Create upload buffer
        std::unique_ptr<dx12_util::Buffer> upload_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                width * height * dx12_util::GetDXGIFormatPixelSize(format), D3D12_HEAP_TYPE_UPLOAD,
                std::wstring(debug_name_prefix + L"_Upload"),
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!upload_buffer)
            return false; // Failure

        // Add upload buffer to resource manager
        *out_upload_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(upload_buffer));
        if (!out_upload_handle->IsValid())
            return false; // Failure

        if (data)
        {
            // Get texture upload pass
            render_graph::RenderPassBase& pass 
                = graphics_service_api.GetRenderPass(render_graph::TextureUploadPassHandle::ID());
            render_graph::TextureUploadPass* texture_upload_pass 
                = dynamic_cast<render_graph::TextureUploadPass*>(&pass);
            assert(texture_upload_pass && "Failed to cast to TextureUploadPass.");

            // Add texture upload task
            render_graph::TextureUploadPass::UploadTask task;
            task.texture_handle = out_handle;
            task.upload_buffer_handle = out_upload_handle;
            task.data = data;
            if (!texture_upload_pass->AddUploadTask(std::move(task)))
                return false; // Failure
        }

        return true; // Success
    });
}

void GraphicsCommandList::CreateWorldBufferForGeometryPass(
    render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size = sizeof(render_graph::geometry_pass::WorldBuffer);

        // Create world buffer
        std::unique_ptr<dx12_util::Buffer> world_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_WorldBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!world_buffer)
            return false; // Failure

        // Initialize buffer data
        render_graph::geometry_pass::WorldBuffer initial_data = {};
        world_buffer->UpdateData(&initial_data, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(world_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateTexture2DsForGeometryPass(
    std::vector<render_graph::ResourceHandles>* out_gbuffer_handles,
    UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix)
{
    AddCommand([
        out_gbuffer_handles, client_width, client_height, back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create G-Buffer textures
        out_gbuffer_handles->clear();
        for (UINT frame_index = 0; frame_index < back_buffer_count; ++frame_index)
        {
            // Resize G-Buffer handles for this frame
            out_gbuffer_handles->resize(back_buffer_count);

            for (size_t buffer_index = 0; buffer_index < (UINT)render_graph::geometry_pass::GBufferIndex::COUNT; ++buffer_index)
            {
                render_graph::ResourceHandle gbuffer_handle;

                D3D12_CLEAR_VALUE clear_value = {};
                clear_value.Format = render_graph::geometry_pass::GBUFFER_FORMATS[buffer_index];
                clear_value.Color[0] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[buffer_index][0];
                clear_value.Color[1] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[buffer_index][1];
                clear_value.Color[2] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[buffer_index][2];
                clear_value.Color[3] = render_graph::geometry_pass::GBUFFER_CLEAR_COLORS[buffer_index][3];

                bool success = true;
                render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
                {
                    // Create texture2D
                    std::unique_ptr<dx12_util::Texture2D> texture
                        = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                            client_width, client_height,
                            render_graph::geometry_pass::GBUFFER_FORMATS[buffer_index], 
                            D3D12_HEAP_TYPE_DEFAULT,
                            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
                            debug_name_prefix + L"_GBuffer_" + std::to_wstring(buffer_index),
                            dx12_util::Device::GetInstance().Get(), &clear_value,
                            nullptr, nullptr, nullptr, nullptr,
                            &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
                    if (!texture)
                    {
                        success = false;
                        return; // Failure
                    }

                    // Add texture to resource manager
                    gbuffer_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(texture));
                    if (!gbuffer_handle.IsValid())
                    {
                        success = false;
                        return; // Failure
                    }

                    // Store G-Buffer handle
                    out_gbuffer_handles->at(frame_index).emplace_back(std::move(gbuffer_handle));
                });

                if (!success)
                    return false; // Failure
            }
        }

        return true; // Success
    });
}

void GraphicsCommandList::CreateDepthStencilForGeometryPass(
    render_graph::ResourceHandles* out_depth_stencil_handles, 
    UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix)
{
    AddCommand([
        out_depth_stencil_handles, client_width, client_height, back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create depth-stencil buffer
        D3D12_CLEAR_VALUE clear_value = {};
        clear_value.Format = render_graph::geometry_pass::DEPTH_STENCIL_FORMAT;
        clear_value.DepthStencil.Depth = render_graph::geometry_pass::DEPTH_CLEAR_VALUE;
        clear_value.DepthStencil.Stencil = render_graph::geometry_pass::STENCIL_CLEAR_VALUE;

        // Clear previous handles
        out_depth_stencil_handles->clear();

        bool success = true;
        for (UINT i = 0; i < back_buffer_count; ++i)
        {
            render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
            {
                std::unique_ptr<dx12_util::Texture2D> depth_stencil
                    = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                        client_width, client_height,
                        render_graph::geometry_pass::DEPTH_STENCIL_FORMAT, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_DEPTH_WRITE,
                        debug_name_prefix + L"_DepthStencil",
                        dx12_util::Device::GetInstance().Get(), &clear_value,
                        &render_graph::geometry_pass::DEPTH_STENCIL_SRV_FORMAT, // SRV format
                        nullptr, nullptr, nullptr,
                        &heap_manager.GetSrvHeapAllocator(), nullptr, &heap_manager.GetDsvHeapAllocator());
                if (!depth_stencil)
                {
                    success = false;
                    return; // Failure
                }

                // Add depth-stencil to resource manager
                render_graph::ResourceHandle depth_stencil_handle 
                    = graphics_service_api.GetResourceAdder().AddResource(std::move(depth_stencil));
                if (!depth_stencil_handle.IsValid())
                {
                    success = false;
                    return; // Failure
                }

                // Store depth-stencil handle
                out_depth_stencil_handles->emplace_back(std::move(depth_stencil_handle));
            });
        }

        if (!success)
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateFullScreenTriangleVertexBufferForComposition(
    render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size 
            = sizeof(render_graph::composition_pass::Vertex) * render_graph::composition_pass::FULL_SCREEN_TRIANGLE_VERTEX_COUNT;

        // Create vertex buffer
        std::unique_ptr<dx12_util::Buffer> vertex_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_FullScreenTriangleVertexBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!vertex_buffer)
            return false; // Failure

        // Initialize buffer data
        vertex_buffer->UpdateData(
            render_graph::composition_pass::FULL_SCREEN_TRIANGLE_VERTICES, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(vertex_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateFullScreenTriangleIndexBufferForComposition(
    render_graph::ResourceHandle* out_handle, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Calculate buffer size
        size_t buffer_size 
            = sizeof(render_graph::composition_pass::Index) * render_graph::composition_pass::FULL_SCREEN_TRIANGLE_INDEX_COUNT;

        // Create index buffer
        std::unique_ptr<dx12_util::Buffer> index_buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                buffer_size, D3D12_HEAP_TYPE_UPLOAD, debug_name_prefix + L"_FullScreenTriangleIndexBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!index_buffer)
            return false; // Failure

        // Initialize buffer data
        index_buffer->UpdateData(
            render_graph::composition_pass::FULL_SCREEN_TRIANGLE_INDICES, buffer_size);

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(index_buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateImguiRenderTargetTextures(
    render_graph::ResourceHandles* out_handles, UINT client_width, UINT client_height, UINT back_buffer_count,
    std::wstring debug_name_prefix)
{
    AddCommand([
        out_handles, client_width, client_height, back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Create render target textures
            for (UINT i = 0; i < back_buffer_count; ++i)
            {
                render_graph::ResourceHandle rt_handle;

                D3D12_CLEAR_VALUE clear_value = {};
                clear_value.Format = render_graph::imgui_pass::TARGET_FORMAT;
                clear_value.Color[0] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[0];
                clear_value.Color[1] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[1];
                clear_value.Color[2] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[2];
                clear_value.Color[3] = render_graph::imgui_pass::TARGET_CLEAR_COLOR[3];

                // Create texture2D
                std::unique_ptr<dx12_util::Texture2D> texture
                    = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                        client_width, client_height,
                        render_graph::imgui_pass::TARGET_FORMAT, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
                        debug_name_prefix + L"_ImGuiRenderTarget_" + std::to_wstring(i),
                        dx12_util::Device::GetInstance().Get(), &clear_value,
                        nullptr, nullptr, nullptr, nullptr,
                        &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
                if (!texture)
                {
                    success = false;
                    return; // Failure
                }

                // Add texture to resource manager
                rt_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(texture));
                if (!rt_handle.IsValid())
                {
                    success = false;
                    return; // Failure
                }

                // Store render target handle
                out_handles->emplace_back(std::move(rt_handle));
            }
        });

        if (!success)
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateImguiContext(
    render_graph::ImguiContextHandle* out_handle, HWND hwnd,
    render_graph::ImguiContext::ContextCreateFunc&& create_func, 
    render_graph::ImguiContext::ContextDestroyFunc&& destroy_func)
{
    AddCommand([
        out_handle, hwnd,
        create_func = std::move(create_func), 
        destroy_func = std::move(destroy_func)](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create ImGui context
        std::unique_ptr<render_graph::ImguiContext> imgui_context
            = render_graph::ImguiContext::CreateInstance<render_graph::ImguiContext>(
                hwnd, std::move(create_func), std::move(destroy_func));
        if (!imgui_context)
            return false; // Failure

        // Add ImGui context to resource manager
        *out_handle = graphics_service_api.GetImguiContextAdder().Add(std::move(imgui_context));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateLightsBuffer(
    std::vector<render_graph::ResourceHandle>* out_handles, 
    std::vector<render_graph::ResourceHandle>* out_upload_handles, 
    size_t back_buffer_count, size_t max_lights, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handles, out_upload_handles, back_buffer_count, max_lights, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Clear output handles
        out_handles->clear();
        out_upload_handles->clear();

        bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            for (size_t i = 0; i < back_buffer_count; ++i)
            {
                render_graph::ResourceHandle buffer_handle;
                render_graph::ResourceHandle upload_handle;

                // Create structured buffers for lights
                std::unique_ptr<dx12_util::StructuredBuffer> buffer
                    = dx12_util::StructuredBuffer::CreateInstance<dx12_util::StructuredBuffer>(
                        sizeof(render_graph::Light::LightBuffer), max_lights, D3D12_HEAP_TYPE_DEFAULT, false,
                        debug_name_prefix + L"_LightsBuffer" + std::to_wstring(i),
                        dx12_util::Device::GetInstance().Get(), &heap_manager.GetSrvHeapAllocator());
                if (!buffer)
                {
                    success = false;
                    return; // Failure
                }

                // Add buffer to resource manager
                buffer_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(buffer));
                if (!buffer_handle.IsValid())
                {
                    success = false;
                    return; // Failure
                }

                // Create upload buffer
                std::unique_ptr<dx12_util::Buffer> upload_buffer
                    = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                        sizeof(render_graph::Light::LightBuffer) * max_lights, D3D12_HEAP_TYPE_UPLOAD,
                        debug_name_prefix + L"_LightsUploadBuffer" + std::to_wstring(i),
                        dx12_util::Device::GetInstance().Get(), nullptr);
                if (!upload_buffer)
                {
                    success = false;
                    return; // Failure
                }

                // Add upload buffer to resource manager
                upload_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(upload_buffer));
                if (!upload_handle.IsValid())
                {
                    success = false;
                    return; // Failure
                }

                // Store handles
                out_handles->emplace_back(std::move(buffer_handle));
                out_upload_handles->emplace_back(std::move(upload_handle));
            }
        });

        if (!success)
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateLightConfigBuffer(
    render_graph::ResourceHandle* out_handle, 
    std::unique_ptr<render_graph::Light::LightConfigBuffer> initial_data, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, data = std::move(initial_data), debug_name_prefix](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create light config buffer
        std::unique_ptr<dx12_util::Buffer> buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                sizeof(render_graph::Light::LightConfigBuffer), D3D12_HEAP_TYPE_UPLOAD,
                debug_name_prefix + L"_LightConfigBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!buffer)
            return false; // Failure

        if (data)
        {
            // Initialize buffer data
            buffer->UpdateData(data.get(), sizeof(render_graph::Light::LightConfigBuffer));
        }

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateRenderTargetTexturesForLightingPasse(
    std::vector<render_graph::ResourceHandles>* out_handles,
    UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handles, client_width, client_height, back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create render target textures
        out_handles->clear();

        bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Resize output handles
            out_handles->resize(back_buffer_count);

            for (UINT frame_index = 0; frame_index < back_buffer_count; ++frame_index)
            {
                for (size_t i = 0; i < (UINT)render_graph::lighting_pass::RenderTargetIndex::COUNT; ++i)
                {
                    render_graph::ResourceHandle rt_handle;

                    D3D12_CLEAR_VALUE clear_value = {};
                    clear_value.Format = render_graph::lighting_pass::RENDER_TARGET_FORMATS[i];
                    clear_value.Color[0] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][0];
                    clear_value.Color[1] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][1];
                    clear_value.Color[2] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][2];
                    clear_value.Color[3] = render_graph::lighting_pass::RENDER_TARGET_CLEAR_COLORS[i][3];

                    // Create texture2D
                    std::unique_ptr<dx12_util::Texture2D> texture
                        = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                            client_width, client_height,
                            render_graph::lighting_pass::RENDER_TARGET_FORMATS[i], D3D12_HEAP_TYPE_DEFAULT,
                            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
                            debug_name_prefix + L"_LightingPassRenderTarget_" + std::to_wstring(i),
                            dx12_util::Device::GetInstance().Get(), &clear_value,
                            nullptr, nullptr, nullptr, nullptr,
                            &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
                    if (!texture)
                    {
                        success = false;
                        return; // Failure
                    }

                    // Add texture to resource manager
                    rt_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(texture));
                    if (!rt_handle.IsValid())
                    {
                        success = false;
                        return; // Failure
                    }

                    // Store render target handle
                    out_handles->at(frame_index).emplace_back(std::move(rt_handle));
                }
            }
        });

        if (!success)
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateShadowCompositionConfigBuffer(
    render_graph::ResourceHandle* out_handle,
    std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> initial_data,
    std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, data = std::move(initial_data), debug_name_prefix](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create shadow composition config buffer
        std::unique_ptr<dx12_util::Buffer> buffer
            = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                sizeof(render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer), D3D12_HEAP_TYPE_UPLOAD,
                debug_name_prefix + L"_ShadowCompositionConfigBuffer",
                dx12_util::Device::GetInstance().Get(), nullptr);
        if (!buffer)
            return false; // Failure

        if (data)
        {
            // Initialize buffer data
            buffer->UpdateData(data.get(), sizeof(render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer));
        }

        // Add buffer to resource manager
        *out_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(buffer));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateRenderTargetTexturesForShadowCompositionPass(
    std::vector<render_graph::ResourceHandles>* out_handles,
    UINT client_width, UINT client_height, UINT back_buffer_count, std::wstring debug_name_prefix)
{
    AddCommand([
        out_handles, client_width, client_height, back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create render target textures
        out_handles->clear();

        bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            // Resize output handles
            out_handles->resize(back_buffer_count);

            for (UINT frame_index = 0; frame_index < back_buffer_count; ++frame_index)
            {
                for (size_t i = 0; i < (UINT)render_graph::shadow_composition_pass::RenderTargetIndex::COUNT; ++i)
                {
                    render_graph::ResourceHandle rt_handle;

                    D3D12_CLEAR_VALUE clear_value = {};
                    clear_value.Format = render_graph::shadow_composition_pass::RENDER_TARGET_FORMATS[i];
                    clear_value.Color[0] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][0];
                    clear_value.Color[1] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][1];
                    clear_value.Color[2] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][2];
                    clear_value.Color[3] = render_graph::shadow_composition_pass::RENDER_TARGET_CLEAR_COLORS[i][3];

                    // Create texture2D
                    std::unique_ptr<dx12_util::Texture2D> texture
                        = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                            client_width, client_height,
                            render_graph::shadow_composition_pass::RENDER_TARGET_FORMATS[i], D3D12_HEAP_TYPE_DEFAULT,
                            D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
                            debug_name_prefix + L"_ShadowCompositionPassRenderTarget_" + std::to_wstring(i),
                            dx12_util::Device::GetInstance().Get(), &clear_value,
                            nullptr, nullptr, nullptr, nullptr,
                            &heap_manager.GetSrvHeapAllocator(), &heap_manager.GetRtvHeapAllocator(), nullptr);
                    if (!texture)
                    {
                        success = false;
                        return; // Failure
                    }

                    // Add texture to resource manager
                    rt_handle = graphics_service_api.GetResourceAdder().AddResource(std::move(texture));
                    if (!rt_handle.IsValid())
                    {
                        success = false;
                        return; // Failure
                    }

                    // Store render target handle
                    out_handles->at(frame_index).emplace_back(std::move(rt_handle));
                }
            }
        });

        if (!success)
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateDirectionalLight(
    render_graph::LightHandle* out_handle,
    std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t back_buffer_count,
    std::wstring debug_name_prefix)
{
    AddCommand([
        out_handle, param = std::move(setup_param), back_buffer_count, debug_name_prefix](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create instance of DirectionalLight
        std::unique_ptr<render_graph::DirectionalLight> light
            = std::make_unique<render_graph::DirectionalLight>();

        // Setup light
        if (!light->Setup(*param))
            return false; // Failure

        {
            // Create view matrix buffer for light
            std::unique_ptr<dx12_util::Buffer> buffer
                = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                    sizeof(DirectX::XMMATRIX), D3D12_HEAP_TYPE_UPLOAD,
                    debug_name_prefix + L"_DirectionalLight_ViewMatrixBuffer",
                    dx12_util::Device::GetInstance().Get(), nullptr);
            if (!buffer)
                return false; // Failure

            // Initialize buffer data
            DirectX::XMMATRIX initial_matrix = DirectX::XMMatrixIdentity();
            buffer->UpdateData(&initial_matrix, sizeof(DirectX::XMMATRIX));

            // Add buffer to resource manager
            render_graph::ResourceHandle buffer_handle
                = graphics_service_api.GetResourceAdder().AddResource(std::move(buffer));
            if (!buffer_handle.IsValid())
                return false; // Failure

            // Set view matrix buffer handle to light
            light->SetViewMatrixBufferHandle(std::move(buffer_handle));
        }

        {
            // Create projection matrix buffer for light
            std::unique_ptr<dx12_util::Buffer> buffer
                = dx12_util::Buffer::CreateInstance<dx12_util::Buffer>(
                    sizeof(DirectX::XMMATRIX), D3D12_HEAP_TYPE_UPLOAD,
                    debug_name_prefix + L"_DirectionalLight_ProjMatrixBuffer",
                    dx12_util::Device::GetInstance().Get(), nullptr);
            if (!buffer)
                return false; // Failure

            // Initialize buffer data
            DirectX::XMMATRIX initial_matrix = DirectX::XMMatrixIdentity();
            buffer->UpdateData(&initial_matrix, sizeof(DirectX::XMMATRIX));

            // Add buffer to resource manager
            render_graph::ResourceHandle buffer_handle
                = graphics_service_api.GetResourceAdder().AddResource(std::move(buffer));
            if (!buffer_handle.IsValid())
                return false; // Failure

            // Set projection matrix buffer handle to light
            light->SetProjMatrixBufferHandle(std::move(buffer_handle));
        }

        // Cast setup_param to DirectionalLightSetupParam
        render_graph::DirectionalLight::SetupParam* directional_light_param
            = dynamic_cast<render_graph::DirectionalLight::SetupParam*>(param.get());
        assert(directional_light_param && "Failed to cast to DirectionalLight::SetupParam.");

		bool success = true;
        render_graph::HeapManager::GetInstance().WithUniqueLock([&](render_graph::HeapManager& heap_manager)
        {
            std::vector<render_graph::ResourceHandle> shadow_map_handles;
            for (UINT i = 0; i < back_buffer_count; ++i)
            {
                // Create shadow map texture for light
                D3D12_CLEAR_VALUE clear_value = {};
                clear_value.Format = render_graph::shadowing_pass::SHADOW_MAP_FORMAT;
                clear_value.DepthStencil.Depth = render_graph::shadowing_pass::SHADOW_MAP_CLEAR_VALUE;
                clear_value.DepthStencil.Stencil = render_graph::shadowing_pass::SHADOW_MAP_STENCIL_CLEAR_VALUE;

                std::unique_ptr<dx12_util::Texture2D> shadow_map
                    = dx12_util::Texture2D::CreateInstance<dx12_util::Texture2D>(
                        directional_light_param->shadow_map_size, directional_light_param->shadow_map_size,
                        render_graph::shadowing_pass::SHADOW_MAP_FORMAT, D3D12_HEAP_TYPE_DEFAULT,
                        D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,
                        debug_name_prefix + L"_DirectionalLight_ShadowMap_" + std::to_wstring(i),
                        dx12_util::Device::GetInstance().Get(), &clear_value,
                        &render_graph::shadowing_pass::SHADOW_MAP_SRV_FORMAT, // SRV format
                        nullptr, nullptr, nullptr,
                        &heap_manager.GetSrvHeapAllocator(), nullptr, &heap_manager.GetDsvHeapAllocator());
                        
                if (!shadow_map)
                {
                    success = false;
					return; // Failure
                }

                // Add shadow map to resource manager
                render_graph::ResourceHandle shadow_map_handle
                    = graphics_service_api.GetResourceAdder().AddResource(std::move(shadow_map));
                if (!shadow_map_handle.IsValid())
                {
					success = false;
					return; // Failure
                }

                // Store shadow map handle
                shadow_map_handles.emplace_back(std::move(shadow_map_handle));
            }

            // Set shadow map handle to light
            light->SetShadowMapHandles(std::move(shadow_map_handles));
        });

        if (!success)
			return false; // Failure

        // Add light to light container
        *out_handle = graphics_service_api.GetLightAdder().AddLight(std::move(light));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreateAmbientLight(
    render_graph::LightHandle* out_handle,
    std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t frame_count)
{
    AddCommand([
        out_handle, param = std::move(setup_param)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create instance of AmbientLight
        std::unique_ptr<render_graph::AmbientLight> light
            = std::make_unique<render_graph::AmbientLight>();

        // Setup light
        if (!light->Setup(*param))
            return false; // Failure

        // Add light to light container
        *out_handle = graphics_service_api.GetLightAdder().AddLight(std::move(light));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CreatePointLight(
    render_graph::LightHandle* out_handle,
    std::unique_ptr<render_graph::Light::SetupParam> setup_param, uint32_t frame_count)
{
    AddCommand([out_handle, param = std::move(setup_param)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Create instance of PointLight
        std::unique_ptr<render_graph::PointLight> light
            = std::make_unique<render_graph::PointLight>();

        // Setup light
        if (!light->Setup(*param))
            return false; // Failure

        // Add light to light container
        *out_handle = graphics_service_api.GetLightAdder().AddLight(std::move(light));
        if (!out_handle->IsValid())
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UploadLight(const render_graph::LightHandle* light_handle)
{
    AddCommand([
        light_handle](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        {
            // Get light upload pass
            render_graph::RenderPassBase& pass 
                = graphics_service_api.GetRenderPass(render_graph::LightUploadPassHandle::ID());
            render_graph::LightUploadPass* light_upload_pass 
                = dynamic_cast<render_graph::LightUploadPass*>(&pass);
            assert(light_upload_pass && "Failed to cast to LightUploadPass.");

            // Add upload light
            light_upload_pass->AddUploadLight(light_handle);
        }

        return true; // Success
    });
}

void GraphicsCommandList::AddLightUploadPassToGraph(
    const render_graph::ResourceHandle* lights_upload_buffer_handle, 
    const render_graph::ResourceHandle* light_config_buffer_handle,
    render_graph::Light::LightConfigBuffer light_config)
{
    AddCommand([
        lights_upload_buffer_handle, light_config_buffer_handle, config = std::move(light_config)]
    (mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get light upload pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::LightUploadPassHandle::ID());
        render_graph::LightUploadPass* light_upload_pass 
            = dynamic_cast<render_graph::LightUploadPass*>(&pass);
        assert(light_upload_pass && "Failed to cast to LightUploadPass.");

        // Set upload buffer and config buffers
        light_upload_pass->SetLightsUploadBufferHandle(lights_upload_buffer_handle);
        light_upload_pass->SetLightConfigBufferHandle(light_config_buffer_handle);

        // Set light config
        light_upload_pass->SetLightConfig(std::move(config));

        // Add light upload pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::LightUploadPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UpdateMatrixBuffer(
    const render_graph::ResourceHandle* buffer_handle, std::unique_ptr<XMMATRIX> matrix_data)
{
    AddCommand([
        buffer_handle, data = std::move(matrix_data)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass& buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass&>(pass);

        // Add buffer update task
        render_graph::BufferUploadPass::UploadTask task;
        task.buffer_handle = buffer_handle;
        task.data = data.get();
        task.size = sizeof(XMMATRIX);
        if (!buffer_update_pass.AddUploadTask(std::move(task)))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UpdateWorldBufferForGeometryPass(
    const render_graph::ResourceHandle* buffer_handle, 
    std::unique_ptr<render_graph::geometry_pass::WorldBuffer> world_buffer_data)
{
    AddCommand([
        buffer_handle, data = std::move(world_buffer_data)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass* buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
        assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

        // Add buffer update task
        render_graph::BufferUploadPass::UploadTask task;
        task.buffer_handle = buffer_handle;
        task.data = data.get();
        task.size = sizeof(render_graph::geometry_pass::WorldBuffer);
        if (!buffer_update_pass->AddUploadTask(std::move(task)))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UpdateMaterialBuffer(
    const render_graph::MaterialHandle* material_handle,
    std::unique_ptr<render_graph::Material::SetupParam> material_setup_param)
{
    AddCommand([
        material_handle, setup_param = std::move(material_setup_param)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

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
            render_graph::Material& material = material_manager.GetMaterial(material_handle);

            // Update material with new setup param
            if (!material.Apply(*setup_param))
            {
                success = false;
                return; // Failure
            }

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

void GraphicsCommandList::UpdateLightBuffer(
    const render_graph::LightHandle* light_handle,
    std::unique_ptr<render_graph::Light::SetupParam> light_setup_param)
{
    AddCommand([
        light_handle, setup_param = std::move(light_setup_param)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass* buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
        assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

        bool success = true;
        render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& light_manager)
        {
            // Get light
            render_graph::Light& light = light_manager.GetLight(light_handle);

            // Update light with new setup param
            if (!light.Apply(*setup_param))
            {
                success = false;
                return; // Failure
            }
        });

        return success;
    });
}

void GraphicsCommandList::UpdateLightViewProjMatrixBuffer(
    const render_graph::LightHandle* light_handle,
    std::unique_ptr<DirectX::XMFLOAT3> light_position, std::unique_ptr<DirectX::XMFLOAT3> light_rotation)
{
    AddCommand([
        light_handle, position = std::move(light_position), rotation = std::move(light_rotation)]
        (mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass* buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
        assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

        bool success = true;
        render_graph::LightManager::GetInstance().WithLock([&](render_graph::LightManager& light_manager)
        {
            // Get light
            render_graph::Light& light = light_manager.GetLight(light_handle);

            // Set position and rotation
            light.SetPosition(*position);
            light.SetRotation(*rotation);

            // Update matrices
            light.UpdateWorldMatrices();
            light.UpdateViewProjMatrix();

            // Get view matrix buffer handle
            const render_graph::ResourceHandle* view_matrix_buffer_handle = light.GetViewMatrixBufferHandle();

            if (view_matrix_buffer_handle->IsValid())
            {
                // Add buffer update task
                render_graph::BufferUploadPass::UploadTask task;
                task.buffer_handle = view_matrix_buffer_handle;
                task.data = &light.GetViewMatrix();
                task.size = sizeof(DirectX::XMMATRIX);
                if (!buffer_update_pass->AddUploadTask(std::move(task)))
                {
                    success = false;
                    return; // Failure
                }
            }

            // Get projection matrix buffer handle
            const render_graph::ResourceHandle* proj_matrix_buffer_handle = light.GetProjMatrixBufferHandle();

            if (proj_matrix_buffer_handle->IsValid())
            {
                // Add buffer update task
                render_graph::BufferUploadPass::UploadTask task;
                task.buffer_handle = proj_matrix_buffer_handle;
                task.data = &light.GetProjMatrix();
                task.size = sizeof(DirectX::XMMATRIX);
                if (!buffer_update_pass->AddUploadTask(std::move(task)))
                {
                    success = false;
                    return; // Failure
                }
            }
        });

        return success;
    });
}

void GraphicsCommandList::UpdateLightsBuffer(
    const render_graph::ResourceHandle* lights_buffer_handle, 
    const render_graph::ResourceHandle* lights_upload_buffer_handle, size_t light_max_count)
{
    AddCommand([
        lights_buffer_handle, lights_upload_buffer_handle, light_max_count]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass* buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
        assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

        // Add buffer update task
        render_graph::BufferUploadPass::StructuredBufferUploadTask task;
        task.buffer_handle = lights_buffer_handle;
        task.upload_buffer_handle = lights_upload_buffer_handle;
        task.size = sizeof(render_graph::Light::LightBuffer) * light_max_count;
        if (!buffer_update_pass->AddUploadTask(std::move(task)))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UpdateShadowCompositionConfigBuffer(
    const render_graph::ResourceHandle* buffer_handle,
    std::unique_ptr<render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer> config_data)
{
    AddCommand([buffer_handle, data = std::move(config_data)](mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get buffer update pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::BufferUploadPassHandle::ID());
        render_graph::BufferUploadPass* buffer_update_pass 
            = dynamic_cast<render_graph::BufferUploadPass*>(&pass);
        assert(buffer_update_pass && "Failed to cast to BufferUploadPass.");

        // Add buffer update task
        render_graph::BufferUploadPass::UploadTask task;
        task.buffer_handle = buffer_handle;
        task.data = data.get();
        task.size = sizeof(render_graph::shadow_composition_pass::ShadowCompositionConfigBuffer);
        if (!buffer_update_pass->AddUploadTask(std::move(task)))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddBufferUploadPassToGraph()
{
    AddCommand([](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Add buffer update pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::BufferUploadPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::UpdateShaderResourceTexture2D(
    const render_graph::ResourceHandle* texture_handle,
    const render_graph::ResourceHandle* upload_handle, const void* data)
{
    AddCommand([texture_handle, upload_handle, data](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get texture upload pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::TextureUploadPassHandle::ID());
        render_graph::TextureUploadPass* texture_upload_pass 
            = dynamic_cast<render_graph::TextureUploadPass*>(&pass);
        assert(texture_upload_pass && "Failed to cast to TextureUploadPass.");

        // Add texture upload task
        render_graph::TextureUploadPass::UploadTask task;
        task.texture_handle = texture_handle;
        task.upload_buffer_handle = upload_handle;
        task.data = data;
        if (!texture_upload_pass->AddUploadTask(std::move(task)))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddTextureUploadPassToGraph()
{
    AddCommand([](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Add texture upload pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::TextureUploadPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::CastShadow(const render_graph::LightHandle* light_handle)
{
    AddCommand([light_handle](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        {
            // Get shadowing pass
            render_graph::RenderPassBase& pass 
                = graphics_service_api.GetRenderPass(render_graph::ShadowingPassHandle::ID());
            render_graph::ShadowingPass* shadowing_pass 
                = dynamic_cast<render_graph::ShadowingPass*>(&pass);
            assert(shadowing_pass && "Failed to cast to ShadowingPass.");

            // Add shadow casting light
            shadowing_pass->AddShadowCastingLight(light_handle);
        }

        {
            // Get shadow composition pass
            render_graph::RenderPassBase& pass 
                = graphics_service_api.GetRenderPass(render_graph::ShadowCompositionPassHandle::ID());
            render_graph::ShadowCompositionPass* shadow_composition_pass 
                = dynamic_cast<render_graph::ShadowCompositionPass*>(&pass);
            assert(shadow_composition_pass && "Failed to cast to ShadowCompositionPass.");

            // Add shadow casting light
            shadow_composition_pass->AddShadowCastingLight(light_handle);
        }

        return true; // Success
    });
}

void GraphicsCommandList::DrawShadowCasterMesh(
    const render_graph::ResourceHandle* world_matrix_buffer_handle,
    const render_graph::ResourceHandle* vertex_buffer_handle,
    const render_graph::ResourceHandle* index_buffer_handle, uint32_t index_count)
{
    AddCommand([
        world_matrix_buffer_handle, vertex_buffer_handle, index_buffer_handle, index_count]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get shadowing pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::ShadowingPassHandle::ID());
        render_graph::ShadowingPass* shadowing_pass 
            = dynamic_cast<render_graph::ShadowingPass*>(&pass);
        assert(shadowing_pass && "Failed to cast to ShadowingPass.");

        // Add draw shadow caster mesh command
        render_graph::ShadowingPass::MeshInfo mesh_info;
        mesh_info.world_matrix_buffer_handle = world_matrix_buffer_handle;
        mesh_info.vertex_buffer_handle = vertex_buffer_handle;
        mesh_info.index_buffer_handle = index_buffer_handle;
        mesh_info.index_count = index_count;
        shadowing_pass->AddShadowCasterMeshInfo(std::move(mesh_info));

        return true; // Success
    });
}

void GraphicsCommandList::AddShadowingPassToGraph(UINT frame_index)
{
    AddCommand([frame_index](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get shadowing pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::ShadowingPassHandle::ID());
        render_graph::ShadowingPass* shadowing_pass 
            = dynamic_cast<render_graph::ShadowingPass*>(&pass);
        assert(shadowing_pass && "Failed to cast to ShadowingPass.");

        // Set frame index
        shadowing_pass->SetCurrentFrameIndex(frame_index);

        // Add shadowing pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::ShadowingPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::DrawMesh(
    const render_graph::ResourceHandle* world_matrix_buffer_handle,
    const render_graph::MaterialHandle* material_handle,
    const render_graph::ResourceHandle* vertex_buffer_handle, 
    const render_graph::ResourceHandle* index_buffer_handle, uint32_t index_count)
{
    AddCommand([
        world_matrix_buffer_handle, material_handle, vertex_buffer_handle, index_buffer_handle, index_count]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get geometry pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::GeometryPassHandle::ID());
        render_graph::GeometryPass& geometry_pass 
            = dynamic_cast<render_graph::GeometryPass&>(pass);

        // Add draw mesh command
        render_graph::GeometryPass::MeshInfo mesh_info;
        mesh_info.world_matrix_buffer_handle = world_matrix_buffer_handle;
        mesh_info.material_handle = material_handle;
        mesh_info.vertex_buffer_handle = vertex_buffer_handle;
        mesh_info.index_buffer_handle = index_buffer_handle;
        mesh_info.index_count = index_count;
        geometry_pass.AddDrawMeshInfo(std::move(mesh_info));

        return true; // Success
    });
}

void GraphicsCommandList::AddGeometryPassToGraph(
    const render_graph::ResourceHandles* gbuffer_handles, 
    const render_graph::ResourceHandle* depth_stencil_handle, 
    const render_graph::ResourceHandle* view_proj_matrix_buffer_handle, 
    D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect)
{
    AddCommand([
        gbuffer_handles, depth_stencil_handle, view_proj_matrix_buffer_handle, viewport, scissor_rect]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get geometry pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::GeometryPassHandle::ID());
        render_graph::GeometryPass& geometry_pass 
            = dynamic_cast<render_graph::GeometryPass&>(pass);

        // Set gbuffers
        assert(
            gbuffer_handles->size() == (size_t)render_graph::geometry_pass::GBufferIndex::COUNT &&
            "G-Buffer handles size does not match GBufferIndex count.");
        geometry_pass.SetGBuffers(gbuffer_handles);

        // Set depth-stencil
        geometry_pass.SetDepthStencil(depth_stencil_handle);

        // Set view-projection matrix buffer
        geometry_pass.SetViewProjMatrixBuffer(view_proj_matrix_buffer_handle);

        // Set viewport
        geometry_pass.SetViewport(viewport);

        // Set scissor rect
        geometry_pass.SetScissorRect(scissor_rect);

        // Add geometry pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::GeometryPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddShadowCompositionPassToGraph(
    const render_graph::ResourceHandles* render_target_handles,
    const render_graph::ResourceHandle* composition_config_buffer_handle,
    const render_graph::ResourceHandle* camera_inv_view_proj_matrix_buffer_handle,
    const render_graph::ResourceHandle* gbuffer_depth_stencil_handle,
    const render_graph::ResourceHandles* gbuffer_render_target_handles,
    const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle,
    const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle,
    D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect)
{
    AddCommand([
        render_target_handles, 
        composition_config_buffer_handle, 
        camera_inv_view_proj_matrix_buffer_handle,
        gbuffer_depth_stencil_handle, 
        gbuffer_render_target_handles, 
        full_screen_triangle_vertex_buffer_handle,
        full_screen_triangle_index_buffer_handle, viewport, scissor_rect]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get shadow composition pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::ShadowCompositionPassHandle::ID());
        render_graph::ShadowCompositionPass* shadow_composition_pass 
            = dynamic_cast<render_graph::ShadowCompositionPass*>(&pass);
        assert(shadow_composition_pass && "Failed to cast to ShadowCompositionPass.");

        // Set render target handles
        shadow_composition_pass->SetRenderTargetHandles(std::move(render_target_handles));

        // Set composition config buffer handle
        shadow_composition_pass->SetShadowCompositionConfigBufferHandle(std::move(composition_config_buffer_handle));

        // Set camera inverse view-projection matrix buffer handle
        shadow_composition_pass->SetCameraInvViewProjMatrixBufferHandle(std::move(camera_inv_view_proj_matrix_buffer_handle));

        // Set full-screen triangle info
        render_graph::ShadowCompositionPass::FullScreenTriangleInfo fs_triangle_info = {};
        fs_triangle_info.vertex_buffer_handle = full_screen_triangle_vertex_buffer_handle;
        fs_triangle_info.index_buffer_handle = full_screen_triangle_index_buffer_handle;
        fs_triangle_info.index_count = render_graph::shadow_composition_pass::FULL_SCREEN_TRIANGLE_INDEX_COUNT;
        shadow_composition_pass->SetFullScreenTriangleInfo(std::move(fs_triangle_info));

        // Set depth-stencil handle from gbuffer
        shadow_composition_pass->SetDepthTextureHandle(std::move(gbuffer_depth_stencil_handle));

        // Set normal texture handle from gbuffer
        shadow_composition_pass->SetNormalTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::NORMAL));

        // Set viewport
        shadow_composition_pass->SetViewport(std::move(viewport));

        // Set scissor rect
        shadow_composition_pass->SetScissorRect(std::move(scissor_rect));

        // Add shadow composition pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::ShadowCompositionPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddLightingPassToGraph(
    const render_graph::ResourceHandles* render_target_handles,
    const render_graph::ResourceHandle* camera_inv_view_proj_matrix_buffer_handle,
    const render_graph::ResourceHandle* light_config_buffer_handle,
    const render_graph::ResourceHandle* lights_buffer_handle,
    const render_graph::ResourceHandle* gbuffer_depth_stencil_handle,
    const render_graph::ResourceHandles* gbuffer_render_target_handles,
    const render_graph::ResourceHandles* shadow_composition_render_target_handles,
    const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle,
    const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle,
    D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect)
{
    AddCommand([
        render_target_handles, 
        camera_inv_view_proj_matrix_buffer_handle,
        light_config_buffer_handle,
        lights_buffer_handle,
        gbuffer_depth_stencil_handle,
        gbuffer_render_target_handles,
        shadow_composition_render_target_handles,
        full_screen_triangle_vertex_buffer_handle,
        full_screen_triangle_index_buffer_handle, viewport, scissor_rect]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get lighting pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::LightingPassHandle::ID());
        render_graph::LightingPass* lighting_pass 
            = dynamic_cast<render_graph::LightingPass*>(&pass);
        assert(lighting_pass && "Failed to cast to LightingPass.");

        // Set render target handles
        lighting_pass->SetRenderTargetTextureHandles(render_target_handles);

        // Set camera inverse view-projection matrix buffer handle
        lighting_pass->SetInvViewProjMatrixBufferHandle(camera_inv_view_proj_matrix_buffer_handle);

        // Set light config buffer handle
        lighting_pass->SetLightConfigBufferHandle(light_config_buffer_handle);

        // Set lights buffer handles
        lighting_pass->SetLightsBufferHandle(lights_buffer_handle);

        // Set full-screen triangle info
        render_graph::LightingPass::FullScreenTriangleInfo fs_triangle_info = {};
        fs_triangle_info.vertex_buffer_handle = full_screen_triangle_vertex_buffer_handle;
        fs_triangle_info.index_buffer_handle = full_screen_triangle_index_buffer_handle;
        fs_triangle_info.index_count = render_graph::lighting_pass::FULL_SCREEN_TRIANGLE_INDEX_COUNT;
        lighting_pass->SetFullScreenTriangleInfo(std::move(fs_triangle_info));

        // Set albedo texture handle from gbuffer
        lighting_pass->SetAlbedoTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::ALBEDO));

        // Set normal texture handle from gbuffer
        lighting_pass->SetNormalTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::NORMAL));

        // Set metalness texture handle from gbuffer
        lighting_pass->SetMetalnessTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::METALNESS));

        // Set roughness texture handle from gbuffer
        lighting_pass->SetRoughnessTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::ROUGHNESS));

        // Set specular texture handle from gbuffer
        lighting_pass->SetSpecularTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::SPECULAR));

        // Set ao texture handle from gbuffer
        lighting_pass->SetAOTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::AO));

        // Set emission texture handle from gbuffer
        lighting_pass->SetEmissionTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::EMISSION));

        // Set mask material texture handle from gbuffer
        lighting_pass->SetMaskMaterialTextureHandle(
            &gbuffer_render_target_handles->at((size_t)render_graph::geometry_pass::GBufferIndex::MASK_MATERIAL));

        // Set mask shadow texture handle from shadow composition pass
        lighting_pass->SetMaskShadowTextureHandle(
            &shadow_composition_render_target_handles->at(
                (UINT)render_graph::shadow_composition_pass::RenderTargetIndex::SHADOW_MASK));

        // Set depth-stencil handle from gbuffer
        lighting_pass->SetDepthStencilTextureHandle(gbuffer_depth_stencil_handle);

        // Set viewport
        lighting_pass->SetViewport(std::move(viewport));

        // Set scissor rect
        lighting_pass->SetScissorRect(std::move(scissor_rect));

        // Add lighting pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::LightingPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddImguiPassToGraph(
    const render_graph::ResourceHandle* target_texture_handle, 
    render_graph::ImguiPass::DrawFunc draw_func, const render_graph::ImguiContextHandle* context_handle)
{
    AddCommand([
        draw_func = std::move(draw_func), target_texture_handle, context_handle]
        (mono_service::ServiceAPI& api) mutable -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get imgui pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::ImguiPassHandle::ID());
        render_graph::ImguiPass& imgui_pass 
            = dynamic_cast<render_graph::ImguiPass&>(pass);

        // Set target texture
        imgui_pass.SetTargetTexture(target_texture_handle);

        // Set draw function
        imgui_pass.SetDrawFunc(std::move(draw_func));

        // Set ImGui context handle
        imgui_pass.SetImguiContext(context_handle);

        // Add imgui pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::ImguiPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::AddCompositionPassToGraph(
    const render_graph::ResourceHandle* swap_chain_handle, DirectX::XMFLOAT4 clear_color,
    const render_graph::ResourceHandle* post_process_texture_handle, 
    const render_graph::ResourceHandle* ui_texture_handle, 
    const render_graph::ResourceHandle* full_screen_triangle_vertex_buffer_handle, 
    const render_graph::ResourceHandle* full_screen_triangle_index_buffer_handle, 
    D3D12_VIEWPORT viewport, D3D12_RECT scissor_rect)
{
    AddCommand([
        swap_chain_handle, clear_color, 
        post_process_texture_handle, 
        ui_texture_handle,
        full_screen_triangle_vertex_buffer_handle, 
        full_screen_triangle_index_buffer_handle, 
        viewport, scissor_rect]
        (mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Get composition pass
        render_graph::RenderPassBase& pass 
            = graphics_service_api.GetRenderPass(render_graph::CompositionPassHandle::ID());
        render_graph::CompositionPass& composition_pass 
            = dynamic_cast<render_graph::CompositionPass&>(pass);

        // Set swap chain
        float render_target_clear_color[4] = {
            clear_color.x, clear_color.y, clear_color.z, clear_color.w};
        composition_pass.SetTargetSwapChain(swap_chain_handle, render_target_clear_color);

        // Set post-process texture
        composition_pass.SetPostProcessTexture(post_process_texture_handle);

        // Set UI texture
        composition_pass.SetUITexture(ui_texture_handle);

        // Set full-screen triangle info
        render_graph::CompositionPass::FullScreenTriangleInfo fs_triangle_info = {};
        fs_triangle_info.vertex_buffer_handle = full_screen_triangle_vertex_buffer_handle;
        fs_triangle_info.index_buffer_handle = full_screen_triangle_index_buffer_handle;
        fs_triangle_info.index_count = render_graph::composition_pass::FULL_SCREEN_TRIANGLE_INDEX_COUNT;
        composition_pass.SetFullScreenTriangleInfo(std::move(fs_triangle_info));

        // Set viewport
        composition_pass.SetViewport(viewport);

        // Set scissor rect
        composition_pass.SetScissorRect(scissor_rect);

        // Add composition pass to render graph
        if (!graphics_service_api.AddRenderPassToGraph(render_graph::CompositionPassHandle::ID()))
            return false; // Failure

        return true; // Success
    });
}

void GraphicsCommandList::DestroyResource(const render_graph::ResourceHandle* resource_handle)
{
    render_graph::ResourceHandle resource_handle_copy = *resource_handle;
    AddCommand([resource_handle_copy](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Destroy resource in resource manager
        graphics_service_api.GetResourceEraser().EraseResource(&resource_handle_copy);

        return true; // Success
    });
}

void GraphicsCommandList::DestroyCommandSet(const render_graph::CommandSetHandle* command_set_handle)
{
    render_graph::CommandSetHandle command_set_handle_copy = *command_set_handle;
    AddCommand([command_set_handle_copy](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Destroy command set in command set manager
        graphics_service_api.GetCommandSetEraser().EraseCommandSet(&command_set_handle_copy);

        return true; // Success
    });
}

void GraphicsCommandList::DestroyImguiContext(const render_graph::ImguiContextHandle* context_handle)
{
    render_graph::ImguiContextHandle context_handle_copy = *context_handle;
    AddCommand([context_handle_copy](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Destroy ImGui context in ImGui context manager
        graphics_service_api.GetImguiContextEraser().Erase(&context_handle_copy);

        return true; // Success
    });
}

void GraphicsCommandList::DestroyMaterial(const render_graph::MaterialHandle* material_handle)
{
    render_graph::MaterialHandle material_handle_copy = *material_handle;
    AddCommand([material_handle_copy](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Destroy material in material manager
        graphics_service_api.GetMaterialEraser().EraseMaterial(&material_handle_copy);

        return true; // Success
    });
}

void GraphicsCommandList::DestroyLight(const render_graph::LightHandle* light_handle)
{
    render_graph::LightHandle light_handle_copy = *light_handle;
    AddCommand([light_handle_copy](mono_service::ServiceAPI& api) -> bool
    {
        // Get graphics service API
        static_assert(
            std::is_base_of<mono_service::ServiceAPI, GraphicsServiceAPI>::value,
            "GraphicsServiceAPI must be derived from ServiceAPI.");
        GraphicsServiceAPI& graphics_service_api = dynamic_cast<GraphicsServiceAPI&>(api);

        // Destroy light in light manager
        graphics_service_api.GetLightEraser().EraseLight(&light_handle_copy);

        return true; // Success
    });
}

} // namespace mono_graphics_service