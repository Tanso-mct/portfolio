#pragma once

#include <vector>
#include <wrl/client.h>
#include <d3d12.h>
#include <DirectXMath.h>

using namespace DirectX;

#include "class_template/instance.h"

#include "render_graph/include/dll_config.h"
#include "render_graph/include/render_graph.h"
#include "render_graph/include/pipeline.h"

namespace render_graph
{

namespace composition_pass
{

// Vertex structure
struct Vertex
{
    XMFLOAT3 position;
};

// Index type
using Index = UINT;

// Index buffer format
constexpr DXGI_FORMAT INDEX_BUFFER_FORMAT = DXGI_FORMAT_R32_UINT;

// Swap chain format
constexpr DXGI_FORMAT SWAP_CHAIN_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
constexpr DXGI_SWAP_EFFECT SWAP_CHAIN_EFFECT = DXGI_SWAP_EFFECT_FLIP_DISCARD;

// Full-screen triangle data
const UINT FULL_SCREEN_TRIANGLE_VERTEX_COUNT = 3;
const Vertex FULL_SCREEN_TRIANGLE_VERTICES[FULL_SCREEN_TRIANGLE_VERTEX_COUNT] =
{
    {XMFLOAT3(-1.0f, -1.0f, 0.0f)},
    {XMFLOAT3(-1.0f,  3.0f, 0.0f)},
    {XMFLOAT3( 3.0f, -1.0f, 0.0f)},
};
const UINT FULL_SCREEN_TRIANGLE_INDEX_COUNT = 3;
const Index FULL_SCREEN_TRIANGLE_INDICES[FULL_SCREEN_TRIANGLE_VERTEX_COUNT] = {1, 2, 0};

} // namespace composition_pass

// Render pass handle for CompositionPass
class RENDER_GRAPH_DLL CompositionPassHandle : 
    public RenderPassHandle<CompositionPassHandle> {};

class CompositionPassAPI :
    public PassAPI
{
public:
    CompositionPassAPI() = default;
    virtual ~CompositionPassAPI() override = default;

    // Get the post-process texture handle
    virtual const ResourceHandle* GetPostProcessTextureHandle() const = 0;

    // Get the UI texture handle
    virtual const ResourceHandle* GetUITextureHandle() const = 0;

    // Get the write access token
    virtual const ResourceAccessToken& GetWriteToken() const = 0;

    // Get the read access token
    virtual const ResourceAccessToken& GetReadToken() const = 0;
};

// Render pass that clears the back buffer
class RENDER_GRAPH_DLL CompositionPass :
    public RenderPassBase,
    public class_template::InstanceGuard<
        CompositionPass,
        class_template::ConstructArgList<>,
        class_template::SetupArgList<std::unique_ptr<Pipeline>>>,
    private CompositionPassAPI
{
public:
    CompositionPass();
    ~CompositionPass();
    bool Setup(std::unique_ptr<Pipeline> composition_pipeline) override;
    bool AddToGraph(RenderGraph& render_graph) override;

    // Set the target swap chain and clear color
    void SetTargetSwapChain(const ResourceHandle* swap_chain_handle, const float clear_color[4]);

    // Set the post-process texture handle
    void SetPostProcessTexture(const ResourceHandle* texture_handle);

    // Set the UI texture handle
    void SetUITexture(const ResourceHandle* texture_handle);

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
    void SetFullScreenTriangleInfo(FullScreenTriangleInfo&& info);

    // Set the viewport for rendering
    void SetViewport(const D3D12_VIEWPORT& viewport);

    // Set the scissor rectangle for rendering
    void SetScissorRect(const D3D12_RECT& scissor_rect);

private:
    PassAPI& GetPassAPI() override { return *this; }
    const PassAPI& GetPassAPI() const override { return *this; }

    const ResourceHandle* GetPostProcessTextureHandle() const override;
    const ResourceHandle* GetUITextureHandle() const override;
    const ResourceAccessToken& GetWriteToken() const override;
    const ResourceAccessToken& GetReadToken() const override;

    std::unique_ptr<Pipeline> composition_pipeline_ = nullptr;

    // Handle of the swap chain
    const ResourceHandle* swap_chain_handle_ = nullptr;

    // Clear color
    float clear_color_[4] = {0.0f, 0.0f, 0.0f, 1.0f};

    // Handle of the post-process texture
    const ResourceHandle* post_process_texture_handle_ = nullptr;

    // Handle of the UI texture
    const ResourceHandle* ui_texture_handle_ = nullptr;

    // Full-screen triangle information
    FullScreenTriangleInfo full_screen_triangle_info_ = FullScreenTriangleInfo();

    // Viewport for rendering
    D3D12_VIEWPORT viewport_ = {};
    bool has_viewport_ = false;

    // Scissor rectangle for rendering
    D3D12_RECT scissor_rect_ = {};
    bool has_scissor_rect_ = false;

    // Current resource write access token
    ResourceAccessToken current_write_token_ = ResourceAccessToken();

    // Current resource read access token
    ResourceAccessToken current_read_token_ = ResourceAccessToken();
    
};

} // namespace render_graph