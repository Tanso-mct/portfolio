#pragma once

#include <DirectXMath.h>
#include <d3d12.h>

#include "render_graph/include/dll_config.h"
#include "render_graph/include/resource_handle.h"
#include "render_graph/include/hlsl_helper.h"
#include "render_graph/include/light_handle.h"

namespace render_graph
{

namespace light
{

constexpr DirectX::XMFLOAT3 DEFAULT_SCENE_CENTER = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
constexpr DirectX::XMFLOAT3 DEFAULT_LIGHT_UP = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
constexpr DirectX::XMFLOAT3 DEFAULT_LIGHT_DIRECTION = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

} // namespace directional_light

// The interface for lights
class RENDER_GRAPH_DLL Light
{
public:
    Light() = default;
    virtual ~Light() = default;

    /*******************************************************************************************************************
     * HLSL definitions
    /******************************************************************************************************************/

    // Light Configuration Constant Buffer
    // cbuffer LightConfigBuffer : register(b0)
    struct LightConfigBuffer
    {
        // Number of lights in the scene
        uint num_lights = 0;

        float3 _padding0; // Padding for alignment

        // Maximum number of lights supported
        uint max_lights = 0;

        float3 _padding0b; // Padding for alignment

        // Screen size (width, height)
        float2 screen_size = float2(0.0f, 0.0f);

        float2 _padding1; // Padding for alignment

        // Shadow intensity
        float shadow_intensity = 1.0f;

        float3 _padding1b; // Padding for alignment

        // Camera world matrix
        matrix camera_world_matrix;

        // Lambert material ID
        uint lambert_material_id = 0;

        float3 _padding2; // Padding for alignment

        // Phong material ID
        uint phong_material_id = 0;

        float3 _padding2b; // Padding for alignment

        // Directional light ID
        uint directional_light_id = 0;

        float3 _padding3; // Padding for alignment

        // Ambient light ID
        uint ambient_light_id = 0;

        float3 _padding4; // Padding for alignment

        // Point light ID
        uint point_light_id = 0;

        float3 _padding5; // Padding for alignment
    };

    inline static const float3 DEFAULT_DIRECTION = float3(0.0f, 0.0f, 1.0f);

    // Structure for light buffer data
    struct LightBuffer
    {
        // Type of the light (e.g., directional, point, spot)
        uint light_type = 0;

        float3 _padding0; // Padding for 16-byte alignment

        // World matrix of the light
        DirectX::XMMATRIX light_world_matrix = DirectX::XMMatrixIdentity();

        // Color of the light
        float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);

        // Intensity of the light
        float intensity = 1.0f;

        float3 _padding1; // Padding for 16-byte alignment

        // Range of the light
        float range = 0.0f;

        float3 _padding2; // Padding for 16-byte alignment
    };

    /*******************************************************************************************************************
     * HLSL definitions end
    /******************************************************************************************************************/

    // Parameter class for setup
    class SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() = default;
    };

    // Setup the material with given parameters
    virtual bool Setup(SetupParam& param) = 0;

    // Apply the material with given parameters
    virtual bool Apply(const SetupParam& param) = 0;

    // Get the unique LightTypeHandleID of the light type
    virtual LightTypeHandleID GetLightTypeHandleID() const = 0;

    // Set position of the light
    void SetPosition(const DirectX::XMFLOAT3& position) { position_ = position; }

    // Set rotation of the light as a quaternion
    void SetRotation(const DirectX::XMFLOAT4& rotation) { rotation_ = rotation; }

    // Set rotation of the light using Euler angles in degrees
    void SetRotation(const DirectX::XMFLOAT3& euler_angles_deg);

    // Update light matrices based on position and rotation
    void UpdateWorldMatrices();

    // Update the view-projection matrix of the light
    virtual void UpdateViewProjMatrix() = 0;

    // Get light buffer data
    const LightBuffer* GetBuffer() const { return &buffer_; }

    // Set shadow map resource handle
    void SetShadowMapHandles(ResourceHandles shadow_map_handle) { shadow_map_handles_ = std::move(shadow_map_handle); }

    // Get shadow map resource handle
    const ResourceHandles* GetShadowMapHandles() const { return &shadow_map_handles_; }

    // Get view matrix of the light
    const DirectX::XMMATRIX& GetViewMatrix() const { return view_matrix_; }

    // Get projection matrix of the light
    const DirectX::XMMATRIX& GetProjMatrix() const { return proj_matrix_; }

    // Set view matrix buffer handle
    void SetViewMatrixBufferHandle(ResourceHandle buffer_handle) { view_matrix_buffer_handle_ = std::move(buffer_handle); }

    // Get view matrix buffer handle
    const ResourceHandle* GetViewMatrixBufferHandle() const { return &view_matrix_buffer_handle_; }

    // Set projection matrix buffer handle
    void SetProjMatrixBufferHandle(ResourceHandle buffer_handle) { proj_matrix_buffer_handle_ = std::move(buffer_handle); }

    // Get projection matrix buffer handle
    const ResourceHandle* GetProjMatrixBufferHandle() const { return &proj_matrix_buffer_handle_; }

    // Set viewport for rendering
    void SetViewport(const D3D12_VIEWPORT& viewport);

    // Get viewport for rendering
    const D3D12_VIEWPORT& GetViewport() const;

    // Set scissor rectangle for rendering
    void SetScissorRect(const D3D12_RECT& scissor_rect);

    // Get scissor rectangle for rendering
    const D3D12_RECT& GetScissorRect() const;

protected:
    // Light buffer data
    LightBuffer buffer_ = LightBuffer();

    // Position of the light in world space
    DirectX::XMFLOAT3 position_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    // Rotation of the light as a quaternion
    DirectX::XMFLOAT4 rotation_ = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    // Shadow map resource handles
    ResourceHandles shadow_map_handles_;

    // View matrix of the light
    DirectX::XMMATRIX view_matrix_ = DirectX::XMMatrixIdentity();

    // Projection matrix of the light
    DirectX::XMMATRIX proj_matrix_ = DirectX::XMMatrixIdentity();

    // View matrix buffer handle
    ResourceHandle view_matrix_buffer_handle_ = ResourceHandle();

    // Projection matrix of the light
    ResourceHandle proj_matrix_buffer_handle_ = ResourceHandle();

private:
    // Viewport for rendering
    D3D12_VIEWPORT viewport_ = {};

    // Flag indicating if viewport is set
    bool has_viewport_ = false;

    // Scissor rectangle for rendering
    D3D12_RECT scissor_rect_ = {};

    // Flag indicating if scissor rect is set
    bool has_scissor_rect_ = false;
};

} // namespace render_graph