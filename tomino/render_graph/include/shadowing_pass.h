#pragma once

#include <vector>
#include <d3d12.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/light_handle.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace shadowing_pass
{

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

// Input layout for shadow mapping
constexpr D3D12_INPUT_ELEMENT_DESC INPUT_LAYOUT[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

// Shadow map formats
constexpr DXGI_FORMAT SHADOW_MAP_FORMAT = DXGI_FORMAT_D24_UNORM_S8_UINT;
constexpr DXGI_FORMAT SHADOW_MAP_SRV_FORMAT = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;

// Depth clear value for shadow map
const float SHADOW_MAP_CLEAR_VALUE = 1.0f;
const UINT8 SHADOW_MAP_STENCIL_CLEAR_VALUE = 0;

} // namespace shadowing_pass

// Render pass handle for ShadowingPass
class RENDER_GRAPH_DLL ShadowingPassHandle : 
    public RenderPassHandle<ShadowingPassHandle> {};

// API interface for LightingPass
class ShadowingPassAPI :
    public PassAPI
{
public:
    ~ShadowingPassAPI() = default;

    // Get drawing light view matrix buffer handle
    virtual const ResourceHandle* GetDrawingLightViewMatrixBufferHandle() const = 0;

    // Get drawing light projection matrix buffer handle
    virtual const ResourceHandle* GetDrawingLightProjMatrixBufferHandle() const = 0;

    // Get drawing world matrix buffer handle
    virtual const ResourceHandle* GetDrawingWorldMatrixBufferHandle() const = 0;

    // Get drawing read token
    virtual const ResourceAccessToken& GetCurrentReadToken() const = 0;

    // Get drawing write token
    virtual const ResourceAccessToken& GetCurrentWriteToken() const = 0;
};

// Render pass for shadow mapping
class RENDER_GRAPH_DLL ShadowingPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        ShadowingPass,
        class_template::ConstructArgList<std::unique_ptr<Pipeline>>,
        class_template::SetupArgList<>>,
    private ShadowingPassAPI
{
public:
    ShadowingPass(std::unique_ptr<Pipeline> shadowing_pipeline);
    ~ShadowingPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Structure to hold mesh buffer information
    struct MeshInfo
    {
        // Handle of the vertex buffer
        const ResourceHandle* vertex_buffer_handle = nullptr;

        // Handle of the index buffer
        const ResourceHandle* index_buffer_handle = nullptr;

        // Number of indices in the index buffer
        UINT index_count = 0;

        // Handle of the world matrix buffer
        const ResourceHandle* world_matrix_buffer_handle = nullptr;
    };

    // Set the current frame index
    void SetCurrentFrameIndex(uint32_t frame_index) { current_frame_index_ = frame_index; }

    // Add a mesh buffer to be rendered
    void AddShadowCasterMeshInfo(MeshInfo&& mesh_buffer_info);

    // Add a light to cast shadows
    void AddShadowCastingLight(const LightHandle* light_handle);

private:
    PassAPI& GetPassAPI() override { return *this; }
    const PassAPI& GetPassAPI() const override { return *this; }
    const ResourceHandle* GetDrawingLightViewMatrixBufferHandle() const override;
    const ResourceHandle* GetDrawingLightProjMatrixBufferHandle() const override;
    const ResourceHandle* GetDrawingWorldMatrixBufferHandle() const override;
    const ResourceAccessToken& GetCurrentReadToken() const override;
    const ResourceAccessToken& GetCurrentWriteToken() const override;

    // Current frame index
    uint32_t current_frame_index_ = 0;

    // Shadowing pipeline
    std::unique_ptr<Pipeline> shadowing_pipeline_;

    // List of light handles to be rendered in this pass
    std::vector<const LightHandle*> render_light_handles_;

    // List of mesh buffers to render
    std::vector<MeshInfo> mesh_infos_;

    // Currently drawing light view matrix buffer handle
    const ResourceHandle* drawing_light_view_matrix_buffer_handle_ = nullptr;

    // Currently drawing light projection matrix buffer handle
    const ResourceHandle* drawing_light_proj_matrix_buffer_handle_ = nullptr;

    // Currently drawing world matrix buffer handle
    const ResourceHandle* drawing_world_matrix_buffer_handle_ = nullptr;

    // Resource access token for reading during drawing
    ResourceAccessToken current_read_token_;

    // Resource access token for writing during drawing
    ResourceAccessToken current_write_token_;
};

} // namespace render_graph