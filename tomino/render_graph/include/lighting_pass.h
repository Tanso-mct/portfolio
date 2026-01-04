#pragma once

#include <vector>
#include <DirectXMath.h>
#include <dxgi.h>

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace lighting_pass
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
    FINAL_COLOR = 0,
    COUNT
};

// Render target formats
constexpr DXGI_FORMAT RENDER_TARGET_FORMATS[(uint32_t)RenderTargetIndex::COUNT] = { DXGI_FORMAT_R16G16B16A16_FLOAT };

// Render target clear colors
constexpr float RENDER_TARGET_CLEAR_COLORS[(uint32_t)RenderTargetIndex::COUNT][4] =
{
    {0.0f, 0.0f, 0.0f, 1.0f}
};

} // namespace lighting_pass

// API interface for LightingPass
class LightingPassAPI :
    public PassAPI
{
public:
    ~LightingPassAPI() = default;

    // Get inverse view-projection matrix buffer handle
    virtual const ResourceHandle* GetInvViewProjMatrixBufferHandle() const = 0;

    // Get light configuration buffer handle
    virtual const ResourceHandle* GetLightConfigBufferHandle() const = 0;

    // Get lights buffer handle
    virtual const ResourceHandle* GetLightsBufferHandle() const = 0;

    // Get albedo texture handle
    virtual const ResourceHandle* GetAlbedoTextureHandle() const = 0;

    // Get normal texture handle
    virtual const ResourceHandle* GetNormalTextureHandle() const = 0;

    // Get metalness texture handle
    virtual const ResourceHandle* GetMetalnessTextureHandle() const = 0;

    // Get roughness texture handle
    virtual const ResourceHandle* GetRoughnessTextureHandle() const = 0;

    // Get specular texture handle
    virtual const ResourceHandle* GetSpecularTextureHandle() const = 0;

    // Get ambient occlusion texture handle
    virtual const ResourceHandle* GetAOTextureHandle() const = 0;

    // Get emission texture handle
    virtual const ResourceHandle* GetEmissionTextureHandle() const = 0;

    // Get mask material texture handle
    virtual const ResourceHandle* GetMaskMaterialTextureHandle() const = 0;

    // Get mask shadow texture handle
    virtual const ResourceHandle* GetMaskShadowTextureHandle() const = 0;

    // Get depth stencil texture handle
    virtual const ResourceHandle* GetDepthStencilTextureHandle() const = 0;

    // Get read access token
    virtual const ResourceAccessToken& GetReadAccessToken() const = 0;

    // Get write access token
    virtual const ResourceAccessToken& GetWriteAccessToken() const = 0;
};

// Render pass handle for LightingPass
class RENDER_GRAPH_DLL LightingPassHandle : 
    public RenderPassHandle<LightingPassHandle> {};

// Render pass that updates buffer resources
class RENDER_GRAPH_DLL LightingPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        LightingPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<std::unique_ptr<Pipeline>>>,
    private LightingPassAPI
{
public:
    LightingPass();
    ~LightingPass();
    bool Setup(std::unique_ptr<Pipeline> lighting_pipeline) override;
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

    // Set viewport for rendering
    void SetViewport(const D3D12_VIEWPORT& viewport);

    // Set scissor rect for rendering
    void SetScissorRect(const D3D12_RECT& scissor_rect);

    // Set final color buffer handle
    void SetRenderTargetTextureHandles(const ResourceHandles* texture_handles);

    // Set inverse view-projection matrix buffer handle
    void SetInvViewProjMatrixBufferHandle(const ResourceHandle* buffer_handle);

    // Set light configuration buffer handle
    void SetLightConfigBufferHandle(const ResourceHandle* buffer_handle);

    // Set lights buffer handle
    void SetLightsBufferHandle(const ResourceHandle* buffer_handle);

    // Set albedo texture handle
    void SetAlbedoTextureHandle(const ResourceHandle* texture_handle);

    // Set normal texture handle
    void SetNormalTextureHandle(const ResourceHandle* texture_handle);

    // Set metalness texture handle
    void SetMetalnessTextureHandle(const ResourceHandle* texture_handle);

    // Set roughness texture handle
    void SetRoughnessTextureHandle(const ResourceHandle* texture_handle);

    // Set specular texture handle
    void SetSpecularTextureHandle(const ResourceHandle* texture_handle);

    // Set ambient occlusion texture handle
    void SetAOTextureHandle(const ResourceHandle* texture_handle);

    // Set emission texture handle
    void SetEmissionTextureHandle(const ResourceHandle* texture_handle);

    // Set mask material texture handle
    void SetMaskMaterialTextureHandle(const ResourceHandle* texture_handle);

    // Set mask shadow texture handle
    void SetMaskShadowTextureHandle(const ResourceHandle* texture_handle);

    // Set depth stencil texture handle
    void SetDepthStencilTextureHandle(const ResourceHandle* depth_stencil_texture_handle);

private:
    PassAPI& GetPassAPI() override { return *this; }
    const PassAPI& GetPassAPI() const override { return *this; }
    const ResourceHandle* GetInvViewProjMatrixBufferHandle() const override;
    const ResourceHandle* GetLightConfigBufferHandle() const override;
    const ResourceHandle* GetLightsBufferHandle() const override;
    const ResourceHandle* GetAlbedoTextureHandle() const override;
    const ResourceHandle* GetNormalTextureHandle() const override;
    const ResourceHandle* GetMetalnessTextureHandle() const override;
    const ResourceHandle* GetRoughnessTextureHandle() const override;
    const ResourceHandle* GetSpecularTextureHandle() const override;
    const ResourceHandle* GetAOTextureHandle() const override;
    const ResourceHandle* GetEmissionTextureHandle() const override;
    const ResourceHandle* GetMaskMaterialTextureHandle() const override;
    const ResourceHandle* GetMaskShadowTextureHandle() const override;
    const ResourceHandle* GetDepthStencilTextureHandle() const override;
    const ResourceAccessToken& GetReadAccessToken() const override;
    const ResourceAccessToken& GetWriteAccessToken() const override;

    // Lighting shader pipeline
    std::unique_ptr<Pipeline> lighting_pipeline_ = nullptr;

    // Full-screen triangle information
    FullScreenTriangleInfo full_screen_triangle_info_ = FullScreenTriangleInfo();

    // Viewport for rendering
    D3D12_VIEWPORT viewport_ = {};
    bool has_viewport_ = false;

    // Scissor rectangle for rendering
    D3D12_RECT scissor_rect_ = {};
    bool has_scissor_rect_ = false;

    // Render target texture handle
    const ResourceHandles* render_target_texture_handles_ = nullptr;

    // Inverse View-projection matrix buffer handle
    const ResourceHandle* inv_view_proj_matrix_buffer_handle_ = nullptr;

    // Light configuration buffer handle
    const ResourceHandle* right_config_buffer_handle_ = nullptr;

    // Lights buffer handle
    const ResourceHandle* rights_buffer_handle_ = nullptr;

    // Handle of albedo texture
    const ResourceHandle* albedo_texture_handle_ = nullptr;

    // Handle of normal texture
    const ResourceHandle* normal_texture_handle_ = nullptr;

    // Handle of metalness texture
    const ResourceHandle* metalness_texture_handle_ = nullptr;

    // Handle of roughness texture
    const ResourceHandle* roughness_texture_handle_ = nullptr;

    // Handle of specular texture
    const ResourceHandle* specular_texture_handle_ = nullptr;

    // Handle of ambient occlusion texture
    const ResourceHandle* ao_texture_handle_ = nullptr;

    // Handle of emission texture
    const ResourceHandle* emission_texture_handle_ = nullptr;

    // Handle of mask material texture
    const ResourceHandle* mask_material_texture_handle_ = nullptr;

    // Handle of mask shadow texture
    const ResourceHandle* mask_shadow_texture_handle_ = nullptr;

    // Handle of the depth texture
    const ResourceHandle* depth_stencil_texture_handle_ = nullptr;

    // Current read access token
    ResourceAccessToken current_read_token_ = ResourceAccessToken();

    // Current write access token
    ResourceAccessToken current_write_token_ = ResourceAccessToken();
};

} // namespace render_graph