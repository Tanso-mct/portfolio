#pragma once

#include <vector>
#include <d3d12.h>
#include <dxgi.h>
#include <DirectXMath.h>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/pipeline.h"
#include "render_graph/include/light_handle.h"
#include "render_graph/include/hlsl_helper.h"

namespace render_graph
{

namespace shadow_composition_pass
{

// Vertex structure
struct Vertex
{
    DirectX::XMFLOAT3 position;
};

constexpr D3D12_INPUT_ELEMENT_DESC INPUT_LAYOUT[] =
{
    {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
};

// Index type
using Index = uint32_t;

// Index buffer format
constexpr DXGI_FORMAT INDEX_BUFFER_FORMAT = DXGI_FORMAT_R32_UINT;

// Full-screen triangle data
const uint32_t FULL_SCREEN_TRIANGLE_VERTEX_COUNT = 3;
const Vertex FULL_SCREEN_TRIANGLE_VERTICES[FULL_SCREEN_TRIANGLE_VERTEX_COUNT] =
{
    {DirectX::XMFLOAT3(-1.0f, -1.0f, 0.0f)},
    {DirectX::XMFLOAT3(-1.0f,  3.0f, 0.0f)},
    {DirectX::XMFLOAT3( 3.0f, -1.0f, 0.0f)},
};
const uint32_t FULL_SCREEN_TRIANGLE_INDEX_COUNT = 3;
const Index FULL_SCREEN_TRIANGLE_INDICES[FULL_SCREEN_TRIANGLE_VERTEX_COUNT] = {1, 2, 0};

// Render target count
enum class RenderTargetIndex : uint32_t
{
    SHADOW_MASK,
    COUNT
};

// Render target formats
constexpr DXGI_FORMAT RENDER_TARGET_FORMATS[(uint32_t)RenderTargetIndex::COUNT] = { DXGI_FORMAT_R16G16B16A16_FLOAT };

// Render target clear colors
constexpr float RENDER_TARGET_CLEAR_COLORS[(uint32_t)RenderTargetIndex::COUNT][4] =
{
    {1.0f, 1.0f, 1.0f, 1.0f}
};

/***********************************************************************************************************************
 * HLSL Definitions
/**********************************************************************************************************************/

// Shadow Composition Configuration Constant Buffer
struct ShadowCompositionConfigBuffer
{
    // Screen size (width, height)
    float2 screen_size = float2(1920.0f, 1080.0f);

    float2 _shadow_composition_config_buffer_padding0; // Padding for alignment

    float shadow_bias = 0.005f;

    float3 _shadow_composition_config_buffer_padding1; // Padding for alignment

    // Slope scaled bias value
    float slope_scaled_bias = 0.02f;

    float3 _shadow_composition_config_buffer_padding2; // Padding for alignment

    float slope_bias_exponent = 2.0f;

    float3 _shadow_composition_config_buffer_padding3; // Padding for alignment
};

} // namespace shadow_composition_pass

// Render pass handle for ShadowCompositionPass
class RENDER_GRAPH_DLL ShadowCompositionPassHandle : 
    public RenderPassHandle<ShadowCompositionPassHandle> {};

// API interface for ShadowCompositionPass
class ShadowCompositionPassAPI :
    public PassAPI
{
public:
    ~ShadowCompositionPassAPI() = default;

    // Get shadow composition configuration buffer handle
    virtual const ResourceHandle* GetShadowCompositionConfigBufferHandle() const = 0;

    // Get current shadow map texture handle
    virtual const ResourceHandle* GetShadowMapTextureHandle() const = 0;

    // Get camera inverse view-projection matrix buffer handle
    virtual const ResourceHandle* GetCameraInvViewProjMatrixBufferHandle() const = 0;

    // Get drawing light view matrix buffer handle
    virtual const ResourceHandle* GetDrawingLightViewMatrixBufferHandle() const = 0;    

    // Get drawing light projection matrix buffer handle
    virtual const ResourceHandle* GetDrawingLightProjMatrixBufferHandle() const = 0;

    // Get depth texture handle
    virtual const ResourceHandle* GetDepthTextureHandle() const = 0;

    // Get normal texture handle
    virtual const ResourceHandle* GetNormalTextureHandle() const = 0;

    // Get read access token
    virtual const ResourceAccessToken& GetReadAccessToken() const = 0;

    // Get write access token
    virtual const ResourceAccessToken& GetWriteAccessToken() const = 0;
};

// Render pass that composes shadows into the main scene
class RENDER_GRAPH_DLL ShadowCompositionPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        ShadowCompositionPass,
        class_template::ConstructArgList<std::unique_ptr<Pipeline>>,
        class_template::SetupArgList<>>,
    private ShadowCompositionPassAPI
{
public:
    ShadowCompositionPass(std::unique_ptr<Pipeline> shadow_composition_pipeline);
    ~ShadowCompositionPass();
    bool Setup() override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Structure to hold full-screen triangle information
    struct FullScreenTriangleInfo
    {
        // Vertex buffer handle
        const ResourceHandle* vertex_buffer_handle = nullptr;

        // Index buffer handle
        const ResourceHandle* index_buffer_handle = nullptr;

        // Number of indices
        UINT index_count = 0;
    };

    // Set full-screen triangle information
    void SetFullScreenTriangleInfo(FullScreenTriangleInfo&& fs_triangle_info);

    // Set the frame index
    void SetFrameIndex(uint32_t index) { frame_index = index; }

    // Add a light whose shadow will be composed
    void AddShadowCastingLight(const LightHandle* light_handle);

    // Set the render target handles
    void SetRenderTargetHandles(const ResourceHandles* texture_handles);

    // Set the shadow composition configuration buffer handle
    void SetShadowCompositionConfigBufferHandle(const ResourceHandle* buffer_handle);

    // Set the camera inverse view-projection matrix buffer handle
    void SetCameraInvViewProjMatrixBufferHandle(const ResourceHandle* buffer_handle);

    // Set depth texture handle
    void SetDepthTextureHandle(const ResourceHandle* texture_handle);

    // Set normal texture handle
    void SetNormalTextureHandle(const ResourceHandle* texture_handle);

    // Set shadow composition configuration
    void SetShadowCompositionConfig(shadow_composition_pass::ShadowCompositionConfigBuffer config);

    // Get the shadow composition configuration
    const shadow_composition_pass::ShadowCompositionConfigBuffer& GetShadowCompositionConfig() const;

    // Set the viewport for shadow composition
    void SetViewport(const D3D12_VIEWPORT& viewport);

    // Set the scissor rect for shadow composition
    void SetScissorRect(const D3D12_RECT& scissor_rect);

private:
    PassAPI& GetPassAPI() override { return *this; }
    const PassAPI& GetPassAPI() const override { return *this; }

    const ResourceHandle* GetShadowCompositionConfigBufferHandle() const override;
    const ResourceHandle* GetShadowMapTextureHandle() const override;
    const ResourceHandle* GetDrawingLightViewMatrixBufferHandle() const override;
    const ResourceHandle* GetDrawingLightProjMatrixBufferHandle() const override;
    const ResourceHandle* GetCameraInvViewProjMatrixBufferHandle() const override;
    const ResourceHandle* GetDepthTextureHandle() const override;
    const ResourceHandle* GetNormalTextureHandle() const override;
    const ResourceAccessToken& GetReadAccessToken() const override;
    const ResourceAccessToken& GetWriteAccessToken() const override;

    // Full-screen triangle information
    FullScreenTriangleInfo fs_triangle_info_ = {};

    // Current frame index
    uint32_t frame_index = 0;

    // Pipeline for shadow composition
    std::unique_ptr<Pipeline> shadow_composition_pipeline_ = nullptr;

    // List of light handles to compose shadows from
    std::vector<const LightHandle*> render_light_handles_;

    // Render target handles
    const ResourceHandles* render_target_handles_ = nullptr;

    // Shadow composition configuration buffer handle
    const ResourceHandle* shadow_composition_config_buffer_handle_ = nullptr;

    // Shadow map texture handle
    const ResourceHandle* shadow_map_texture_handle_ = nullptr;

    // Camera inverse view-projection matrix buffer handle
    const ResourceHandle* camera_inv_view_proj_matrix_buffer_handle_ = nullptr;

    // Depth texture handle
    const ResourceHandle* depth_texture_handle_ = nullptr;

    // Normal texture handle
    const ResourceHandle* normal_texture_handle_ = nullptr;

    // Shadow composition configuration buffer
    shadow_composition_pass::ShadowCompositionConfigBuffer shadow_composition_config_ = {};

    // Viewport for shadow composition
    D3D12_VIEWPORT viewport_ = {};

    // Indicates if viewport is set
    bool has_viewport_ = false;

    // Scissor rect for shadow composition
    D3D12_RECT scissor_rect_ = {};

    // Indicates if scissor rect is set
    bool has_scissor_rect_ = false;

    // Handle of the drawing light view matrix buffer handle
    const ResourceHandle* drawing_light_view_matrix_buffer_handle_ = nullptr;

    // Handle of the drawing light projection matrix buffer handle
    const ResourceHandle* drawing_light_proj_matrix_buffer_handle_ = nullptr;

    // Read access token
    ResourceAccessToken current_read_token_ = {};

    // Write access token
    ResourceAccessToken current_write_token_ = {};
};

} // namespace render_graph