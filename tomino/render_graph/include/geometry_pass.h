#pragma once

#include <vector>
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/material_handle.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace geometry_pass
{

constexpr D3D12_INPUT_ELEMENT_DESC INPUT_LAYOUT[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

// G-buffer identifiers
enum class GBufferIndex : UINT
{
    ALBEDO,
    NORMAL,
    AO,
    SPECULAR,
    ROUGHNESS,
    METALNESS,
    EMISSION,
    MASK_MATERIAL,
    COUNT,
};

// G-buffer formats
const DXGI_FORMAT GBUFFER_FORMATS[(UINT)GBufferIndex::COUNT] = 
{
    DXGI_FORMAT_R8G8B8A8_UNORM, // Albedo format
    DXGI_FORMAT_R16G16B16A16_FLOAT, // Normal format
    DXGI_FORMAT_R8_UNORM, // AO format
    DXGI_FORMAT_R8G8B8A8_UNORM, // Specular format
    DXGI_FORMAT_R8_UNORM, // Roughness format
    DXGI_FORMAT_R8_UNORM, // Metalness format
    DXGI_FORMAT_R8G8B8A8_UNORM, // Emission format
    DXGI_FORMAT_R32_UINT, // Mask material format
};

// G-buffer clear colors
const float GBUFFER_CLEAR_COLORS[(UINT)GBufferIndex::COUNT][4] = 
{
    { 0.0f, 0.0f, 0.0f, 1.0f }, // Albedo clear color
    { 0.5f, 0.5f, 1.0f, 1.0f },  // Normal clear color
    { 1.0f, 1.0f, 1.0f, 1.0f },  // AO clear color
    { 0.0f, 0.0f, 0.0f, 1.0f },  // Specular clear color
    { 1.0f, 1.0f, 1.0f, 1.0f },  // Roughness clear color
    { 0.0f, 0.0f, 0.0f, 0.0f },  // Metalness clear color
    { 0.0f, 0.0f, 0.0f, 1.0f },  // Emission clear color
    { 0.0f, 0.0f, 0.0f, 0.0f },  // Mask material clear color
};

// Depth stencil format
constexpr DXGI_FORMAT DEPTH_STENCIL_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
constexpr DXGI_FORMAT DEPTH_STENCIL_SRV_FORMAT = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

// Depth clear value
const float DEPTH_CLEAR_VALUE = 1.0f;
const UINT8 STENCIL_CLEAR_VALUE = 0;

// Vertex structure
struct Vertex
{
    DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    DirectX::XMFLOAT2 texcoord = DirectX::XMFLOAT2(0.0f, 0.0f);
    DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
};

// Index type
using Index = uint32_t;

// Index buffer format
constexpr DXGI_FORMAT INDEX_BUFFER_FORMAT = DXGI_FORMAT_R32_UINT;

// World matrix buffer structure
struct WorldBuffer
{
    DirectX::XMMATRIX world_matrix = DirectX::XMMatrixIdentity();
    DirectX::XMMATRIX world_inverse_transpose = DirectX::XMMatrixIdentity();
};

} // namespace geometry_pass

// Render pass handle for GeometryPass
class RENDER_GRAPH_DLL GeometryPassHandle : 
    public RenderPassHandle<GeometryPassHandle> {};

// API interface for GeometryPass
class RENDER_GRAPH_DLL GeometryPassAPI :
    public PassAPI
{
public:
    virtual ~GeometryPassAPI() override = default;

    // Get view-projection matrix buffer handle
    virtual const ResourceHandle* GetViewProjMatrixBufferHandle() const = 0;

    // Get drawing world matrix buffer handle
    virtual const ResourceHandle* GetDrawingWorldMatrixBufferHandle() const = 0;

    // Get drawing material handle
    virtual const MaterialHandle* GetDrawingMaterialHandle() const = 0;

    // Get drawing read token
    virtual const ResourceAccessToken& GetCurrentReadToken() const = 0;

    // Get drawing write token
    virtual const ResourceAccessToken& GetCurrentWriteToken() const = 0;
};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL GeometryPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        GeometryPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<std::unordered_map<MaterialTypeHandleID, std::unique_ptr<Pipeline>>&&>>,
    private GeometryPassAPI
{
public:
    GeometryPass();
    ~GeometryPass();

    using PipelineMap = std::unordered_map<MaterialTypeHandleID, std::unique_ptr<Pipeline>>;
    bool Setup(PipelineMap&& pipelines) override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Set a G-buffer texture resource handle
    void SetGBuffers(const ResourceHandles* texture_handles);

    // Set the depth stencil texture resource handle
    void SetDepthStencil(const ResourceHandle* depth_stencil_texture_handle);

    // Structure to hold mesh buffer information
    struct MeshInfo
    {
        // Handle of the vertex buffer
        const ResourceHandle* vertex_buffer_handle = nullptr;

        // Handle of the index buffer
        const ResourceHandle* index_buffer_handle = nullptr;

        // Number of indices in the index buffer
        UINT index_count = 0;

        // Material handle for the mesh
        const MaterialHandle* material_handle = nullptr;

        // Handle of the world matrix buffer
        const ResourceHandle* world_matrix_buffer_handle = nullptr;
    };

    // Add a mesh buffer to be rendered
    void AddDrawMeshInfo(MeshInfo&& mesh_buffer_info);

    // Set view-projection matrix buffer handle
    void SetViewProjMatrixBuffer(const ResourceHandle* buffer_handle);

    // Set the viewport for rendering
    void SetViewport(const D3D12_VIEWPORT& viewport);

    // Set the scissor rectangle for rendering
    void SetScissorRect(const D3D12_RECT& scissor_rect);

private:
    PassAPI& GetPassAPI() override { return *this; }
    const PassAPI& GetPassAPI() const override { return *this; }

    const ResourceHandle* GetViewProjMatrixBufferHandle() const override;
    const ResourceHandle* GetDrawingWorldMatrixBufferHandle() const override;
    const MaterialHandle* GetDrawingMaterialHandle() const override;
    const ResourceAccessToken& GetCurrentReadToken() const override;
    const ResourceAccessToken& GetCurrentWriteToken() const override;

    // Pipelines for different material types
    PipelineMap pipeline_map_;

    // Handles of the G-buffer textures
    const ResourceHandles* gbuffer_texture_handles_ = nullptr;

    // Handle of the depth texture
    const ResourceHandle* depth_stencil_texture_handle_ = nullptr;

    // Viewport for rendering
    D3D12_VIEWPORT viewport_ = {};
    bool has_viewport_ = false;

    // Scissor rectangle for rendering
    D3D12_RECT scissor_rect_ = {};
    bool has_scissor_rect_ = false;

    // List of mesh buffers to render
    std::vector<MeshInfo> mesh_infos_;

    // View-projection matrix buffer handle
    const ResourceHandle* view_proj_matrix_buffer_handle_ = nullptr;

    // Currently drawing world matrix buffer handle
    const ResourceHandle* drawing_world_matrix_buffer_handle_ = nullptr;

    // Currently drawing material handle
    const MaterialHandle* drawing_material_handle_ = nullptr;

    // Resource access token for reading during drawing
    ResourceAccessToken current_read_token_;

    // Resource access token for writing during drawing
    ResourceAccessToken current_write_token_;
};

} // namespace render_graph