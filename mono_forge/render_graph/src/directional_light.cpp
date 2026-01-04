#include "render_graph/src/pch.h"
#include "render_graph/include/directional_light.h"

using namespace DirectX;

namespace render_graph
{

bool DirectionalLight::Setup(Light::SetupParam& param)
{
    // Cast to DirectionalLight::SetupParam
    SetupParam* dir_light_param = dynamic_cast<SetupParam*>(&param);
    assert(dir_light_param != nullptr && "Invalid SetupParam type for DirectionalLight.");

    // Store light buffer data
    buffer_.light_type = DirectionalLightTypeHandle::ID();
    buffer_.color = dir_light_param->color;
    buffer_.intensity = dir_light_param->intensity;
    distance_ = dir_light_param->distance;
    ortho_width_ = dir_light_param->ortho_width;
    ortho_height_ = dir_light_param->ortho_height;
    near_z_ = dir_light_param->near_z;
    far_z_ = dir_light_param->far_z;
    shadow_map_size_ = dir_light_param->shadow_map_size;

    // Initialize viewport
    D3D12_VIEWPORT viewport = D3D12_VIEWPORT();
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = static_cast<float>(shadow_map_size_);
    viewport.Height = static_cast<float>(shadow_map_size_);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    // Set viewport
    SetViewport(viewport);

    // Initialize scissor rectangle
    D3D12_RECT scissor_rect = D3D12_RECT();
    scissor_rect.left = 0;
    scissor_rect.top = 0;
    scissor_rect.right = static_cast<LONG>(shadow_map_size_);
    scissor_rect.bottom = static_cast<LONG>(shadow_map_size_);

    // Set scissor rectangle
    SetScissorRect(scissor_rect);

    return true; // Setup successful
}

bool DirectionalLight::Apply(const Light::SetupParam& param)
{
    // Cast to DirectionalLight::SetupParam
    const SetupParam* dir_light_param = dynamic_cast<const SetupParam*>(&param);
    assert(dir_light_param != nullptr && "Invalid SetupParam type for DirectionalLight.");

    // Update light buffer data
    buffer_.color = dir_light_param->color;
    buffer_.intensity = dir_light_param->intensity;
    distance_ = dir_light_param->distance;
    ortho_width_ = dir_light_param->ortho_width;
    ortho_height_ = dir_light_param->ortho_height;
    near_z_ = dir_light_param->near_z;
    far_z_ = dir_light_param->far_z;

    return true; // Apply successful
}

LightTypeHandleID DirectionalLight::GetLightTypeHandleID() const
{
    return DirectionalLightTypeHandle::ID();
}

void DirectionalLight::UpdateViewProjMatrix()
{
    // Store direction and up vectors
    XMVECTOR up_vec = XMLoadFloat3(&light::DEFAULT_LIGHT_UP);
    XMVECTOR forward_vec = XMLoadFloat3(&light::DEFAULT_LIGHT_DIRECTION);

    // Rotate direction and up vectors by the light's rotation
	XMVECTOR rot_vec = XMLoadFloat4(&rotation_);
    XMVECTOR rotated_forward_vec = XMVector3Rotate(forward_vec, rot_vec);
    XMVECTOR rotated_up_vec = XMVector3Rotate(up_vec, rot_vec);

    // Store scene center vector
    XMVECTOR scene_center_vec = XMLoadFloat3(&light::DEFAULT_SCENE_CENTER);

    // Calculate light position based on scene center and rotated direction
    XMVECTOR pos_vec = XMVectorAdd(
        scene_center_vec, XMVectorScale(rotated_forward_vec, -distance_));

    // Create view matrix
    XMMATRIX view_matrix = XMMatrixLookAtLH(pos_vec, scene_center_vec, rotated_up_vec);

    // Create orthographic projection matrix
    XMMATRIX proj_matrix = XMMatrixOrthographicLH(ortho_width_, ortho_height_, near_z_, far_z_);

    // Transpose for HLSL and store the view and projection matrices
    view_matrix_ = XMMatrixTranspose(view_matrix);
    proj_matrix_ = XMMatrixTranspose(proj_matrix);
}

} // namespace render_graph