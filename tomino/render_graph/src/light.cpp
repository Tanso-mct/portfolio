#include "render_graph/src/pch.h"
#include "render_graph/include/light.h"

using namespace DirectX;

namespace render_graph
{

// Helper function to create world matrix from position and rotation
XMMATRIX CreateMatrix(const XMFLOAT3& position, const XMFLOAT4& rotation)
{
    DirectX::XMVECTOR pos_vec = DirectX::XMLoadFloat3(&position);
    DirectX::XMVECTOR rot_vec = DirectX::XMLoadFloat4(&rotation);
    DirectX::XMMATRIX translation_matrix = DirectX::XMMatrixTranslationFromVector(pos_vec);
    DirectX::XMMATRIX rotation_matrix = DirectX::XMMatrixRotationQuaternion(rot_vec);
    return rotation_matrix * translation_matrix;
}

void Light::SetRotation(const DirectX::XMFLOAT3& euler_angles_deg)
{
    // Convert Euler angles from degrees to radians
    XMFLOAT3 euler_angles_rad = XMFLOAT3(
        XMConvertToRadians(euler_angles_deg.x),
        XMConvertToRadians(euler_angles_deg.y),
        XMConvertToRadians(euler_angles_deg.z));

    // Create quaternion from Euler angles
    XMVECTOR rot_vec = XMQuaternionRotationRollPitchYaw(
        euler_angles_rad.x, euler_angles_rad.y, euler_angles_rad.z);

    // Store rotation
    XMStoreFloat4(&rotation_, rot_vec);
}

void Light::UpdateWorldMatrices()
{
    // Update light world matrix
    buffer_.light_world_matrix = CreateMatrix(position_, rotation_);

    // Transpose matrices for HLSL
    buffer_.light_world_matrix = XMMatrixTranspose(buffer_.light_world_matrix);
}

void Light::SetViewport(const D3D12_VIEWPORT& viewport)
{
    viewport_ = viewport;
    has_viewport_ = true;
}

const D3D12_VIEWPORT& Light::GetViewport() const
{
    assert(has_viewport_ && "Viewport has not been set.");
    return viewport_;
}

void Light::SetScissorRect(const D3D12_RECT& scissor_rect)
{
    scissor_rect_ = scissor_rect;
    has_scissor_rect_ = true;
}

const D3D12_RECT& Light::GetScissorRect() const
{
    assert(has_scissor_rect_ && "Scissor rectangle has not been set.");
    return scissor_rect_;
}

} // namespace render_graph