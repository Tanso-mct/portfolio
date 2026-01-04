#include "mono_transform_extension/src/pch.h"
#include "mono_transform_extension/include/transform_component.h"

using namespace DirectX;

#include "mono_transform_service/include/transform_service_command_list.h"
#include "mono_transform_service/include/transform_service_view.h"

namespace mono_transform_extension
{

void XMQuaternionToEulerAngles(const XMVECTOR& q, float& pitch, float& yaw, float& roll)
{
    XMMATRIX m = XMMatrixRotationQuaternion(q);

    // Decomposition of Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)

    // Pitch
    pitch = asinf(-m.r[2].m128_f32[1]); // -m32

    // Yaw, Roll
    if (cosf(pitch) > 1e-6) 
    {
        yaw  = atan2f(m.r[2].m128_f32[0], m.r[2].m128_f32[2]); // m31, m33
        roll = atan2f(m.r[0].m128_f32[1], m.r[1].m128_f32[1]); // m12, m22
    } 
    else 
    {
        // When gimbal locked
        yaw  = atan2f(-m.r[0].m128_f32[2], m.r[0].m128_f32[0]);
        roll = 0.0f;
    }
}

TransformComponent::TransformComponent(std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy) :
    transform_service_proxy_(std::move(transform_service_proxy))
{
}

TransformComponent::~TransformComponent()
{
    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Destroy transform
    transform_command_list->DestroyTransform(transform_handle_);

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));
}

bool TransformComponent::Setup(ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    TransformComponent::SetupParam* transform_component_param
        = dynamic_cast<TransformComponent::SetupParam*>(&param);
    assert(transform_component_param != nullptr && "Invalid setup param type for TransformComponent");

    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Convert rotation from Euler angles (degrees) to radians
    XMFLOAT3 rotation_radians =XMFLOAT3(
        XMConvertToRadians(transform_component_param->rotation.x),
        XMConvertToRadians(transform_component_param->rotation.y),
        XMConvertToRadians(transform_component_param->rotation.z));

    // Convert Euler angles to quaternion
    XMVECTOR euler_vector = XMLoadFloat3(&rotation_radians);
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(euler_vector);
    XMFLOAT4 rotation_quaternion;
    XMStoreFloat4(&rotation_quaternion, quaternion);

    if (!transform_handle_.IsValid())
    {
        // Create transform with initial TRS
        transform_command_list->CreateTransform(
            transform_handle_,
            transform_component_param->position, rotation_quaternion, transform_component_param->scale);
    }
    else
    {
        // Update existing transform with initial TRS
        transform_command_list->UpdateWorldTRS(
            transform_handle_,
            transform_component_param->position, rotation_quaternion, transform_component_param->scale);
    }

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));

    return true; // Success
}

bool TransformComponent::Apply(const ecs::Component::SetupParam& param)
{
    // Dynamic cast to SetupParam
    const TransformComponent::SetupParam* transform_component_param
        = dynamic_cast<const TransformComponent::SetupParam*>(&param);
    assert(transform_component_param != nullptr && "Invalid setup param type for TransformComponent");

    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Convert rotation from Euler angles (degrees) to radians
    XMFLOAT3 rotation_radians =XMFLOAT3(
        XMConvertToRadians(transform_component_param->rotation.x),
        XMConvertToRadians(transform_component_param->rotation.y),
        XMConvertToRadians(transform_component_param->rotation.z));

    // Convert Euler angles to quaternion
    XMVECTOR euler_vector = XMLoadFloat3(&rotation_radians);
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(euler_vector);
    XMFLOAT4 rotation_quaternion;
    XMStoreFloat4(&rotation_quaternion, quaternion);

    // Update world TRS
    transform_command_list->UpdateWorldTRS(
        transform_handle_,
        transform_component_param->position, rotation_quaternion, transform_component_param->scale);

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));

    return true; // Success
}

ecs::ComponentID TransformComponent::GetID() const
{
    return TransformComponentHandle::ID();
}

XMFLOAT3 TransformComponent::GetWorldPosition() const
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = transform_service_proxy_->CreateView();
    mono_transform_service::TransformServiceView* transform_view
        = dynamic_cast<mono_transform_service::TransformServiceView*>(service_view.get());
    assert(transform_view != nullptr && "Failed to create TransformServiceView");

    // Get world position from transform service view
    return transform_view->GetTranslation(transform_handle_);
}

void TransformComponent::SetWorldPosition(const XMFLOAT3& position)
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Update world translation
    transform_command_list->UpdateWorldTranslation(transform_handle_, position);

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));   
}

XMFLOAT4 TransformComponent::GetWorldRotation() const
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = transform_service_proxy_->CreateView();
    mono_transform_service::TransformServiceView* transform_view
        = dynamic_cast<mono_transform_service::TransformServiceView*>(service_view.get());
    assert(transform_view != nullptr && "Failed to create TransformServiceView");

    // Get world rotation from transform service view
    return transform_view->GetRotation(transform_handle_);
}

XMFLOAT3 TransformComponent::GetWorldEulerAngles() const
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = transform_service_proxy_->CreateView();
    mono_transform_service::TransformServiceView* transform_view
        = dynamic_cast<mono_transform_service::TransformServiceView*>(service_view.get());
    assert(transform_view != nullptr && "Failed to create TransformServiceView");

    // Get world rotation from transform service view
    const XMFLOAT4& rotation_quaternion = transform_view->GetRotation(transform_handle_);
    XMVECTOR q = XMLoadFloat4(&rotation_quaternion);

    // Convert quaternion to Euler angles (radians)
    XMFLOAT3 euler_angles;
    XMQuaternionToEulerAngles(q, euler_angles.x, euler_angles.y, euler_angles.z);

    // Convert to degrees
    euler_angles.x = XMConvertToDegrees(euler_angles.x);
    euler_angles.y = XMConvertToDegrees(euler_angles.y);
    euler_angles.z = XMConvertToDegrees(euler_angles.z);

    return euler_angles; // Return Euler angles in radians
}

void TransformComponent::SetWorldRotation(const XMFLOAT4& rotation)
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Update world rotation
    transform_command_list->UpdateWorldRotation(transform_handle_, rotation);

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));   
}

void TransformComponent::SetWorldRotation(const XMFLOAT3& euler_angles)
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Convert degrees to radians
    XMFLOAT3 euler_angles_radians = XMFLOAT3(
        XMConvertToRadians(euler_angles.x),
        XMConvertToRadians(euler_angles.y),
        XMConvertToRadians(euler_angles.z));

    // Convert Euler angles to quaternion
    XMVECTOR euler_vector = XMLoadFloat3(&euler_angles_radians);
    XMVECTOR quaternion = XMQuaternionRotationRollPitchYawFromVector(euler_vector);
    XMFLOAT4 rotation;
    XMStoreFloat4(&rotation, quaternion);

    // Set world rotation using quaternion
    SetWorldRotation(rotation);
}

XMFLOAT3 TransformComponent::GetWorldScale() const
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = transform_service_proxy_->CreateView();
    mono_transform_service::TransformServiceView* transform_view
        = dynamic_cast<mono_transform_service::TransformServiceView*>(service_view.get());
    assert(transform_view != nullptr && "Failed to create TransformServiceView");

    // Get world scale from transform service view
    return transform_view->GetScale(transform_handle_);
}

void TransformComponent::SetWorldScale(const XMFLOAT3& scale)
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create transform service command list
    std::unique_ptr<mono_service::ServiceCommandList> command_list
        = transform_service_proxy_->CreateCommandList();
    mono_transform_service::TransformServiceCommandList* transform_command_list
        = dynamic_cast<mono_transform_service::TransformServiceCommandList*>(command_list.get());
    assert(transform_command_list != nullptr && "Failed to create TransformServiceCommandList");

    // Update world scale
    transform_command_list->UpdateWorldScale(transform_handle_, scale);

    // Submit command list
    transform_service_proxy_->SubmitCommandList(std::move(command_list));
}

XMMATRIX TransformComponent::GetWorldMatrix() const
{
    assert(transform_handle_.IsValid() && "Transform handle is not valid");

    // Create service view
    std::unique_ptr<mono_service::ServiceView> service_view
        = transform_service_proxy_->CreateView();
    mono_transform_service::TransformServiceView* transform_view
        = dynamic_cast<mono_transform_service::TransformServiceView*>(service_view.get());
    assert(transform_view != nullptr && "Failed to create TransformServiceView");

    // Get world matrix from transform service view
    return transform_view->GetWorldMatrix(transform_handle_);
}

} // namespace mono_transform_extension