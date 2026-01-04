#include "wbp_transform/src/pch.h"
#include "wbp_transform/include/component_transform.h"

using namespace DirectX;

const WBP_TRANSFORM_API size_t &wbp_transform::TransformComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

namespace
{

    void XMQuaternionToEulerAngles(FXMVECTOR q, float* pitch, float* yaw, float* roll)
    {
        XMFLOAT4 qf;
        XMStoreFloat4(&qf, q);

        // Calculate the angular radians of each axis
        float ysqr = qf.y * qf.y;

        // roll (X)
        float t0 = +2.0f * (qf.w * qf.x + qf.y * qf.z);
        float t1 = +1.0f - 2.0f * (qf.x * qf.x + ysqr);
        *roll = std::atan2(t0, t1);

        // pitch (Y)
        float t2 = +2.0f * (qf.w * qf.y - qf.z * qf.x);
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        *pitch = std::asin(t2);

        // yaw (Z)
        float t3 = +2.0f * (qf.w * qf.z + qf.x * qf.y);
        float t4 = +1.0f - 2.0f * (ysqr + qf.z * qf.z);
        *yaw = std::atan2(t3, t4);
    }

}

wbp_transform::TransformComponent::TransformComponent()
{
    position_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    previousPosition_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    localPosition_ = XMFLOAT3(0.0f, 0.0f, 0.0f);

    quatRotation_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    previousRotation_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    quatLocalRotation_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    scale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
    previousLocalScale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
    localScale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
}

const size_t &wbp_transform::TransformComponent::GetID() const
{
    return TransformComponentID();
}

XMVECTOR wbp_transform::TransformComponent::GetUp() const
{
    XMVECTOR quat = XMLoadFloat4(&quatRotation_);
    return XMVector3Rotate(XMLoadFloat3(&XM_UP), quat);
}

XMVECTOR wbp_transform::TransformComponent::GetForward() const
{
    XMVECTOR quat = XMLoadFloat4(&quatRotation_);
    return XMVector3Rotate(XMLoadFloat3(&XM_FORWARD), quat);
}

XMVECTOR wbp_transform::TransformComponent::GetRight() const
{
    XMVECTOR quat = XMLoadFloat4(&quatRotation_);
    return XMVector3Rotate(XMLoadFloat3(&XM_RIGHT), quat);
}

void wbp_transform::TransformComponent::SetPosition(const DirectX::XMFLOAT3 &pos)
{
    previousPosition_ = position_;
    position_ = pos;

    // If Previous is before initialization, set current value as previous value
    if (!previousPosInitialized_)
    {
        previousPosInitialized_ = true;
        previousPosition_ = position_;
    }
}

void wbp_transform::TransformComponent::SetLocalPosition(const DirectX::XMFLOAT3 &localPos)
{
    localPosition_ = localPos;
    isDirty_ = true;
}

void wbp_transform::TransformComponent::SetRotation(const DirectX::XMFLOAT3 &pos)
{
    previousRotation_ = quatRotation_;

    float pitch = XMConvertToRadians(pos.x);
    float yaw   = XMConvertToRadians(pos.y);
    float roll  = XMConvertToRadians(pos.z);
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&quatRotation_, q);

    // If Previous is before initialization, set current value as previous value
    if (!previousQuatLocalRotInitialized_)
    {
        previousQuatLocalRotInitialized_ = true;
        previousRotation_ = quatRotation_;
    }
}

void wbp_transform::TransformComponent::SetLocalRotation(const DirectX::XMFLOAT3 &localRot)
{
    float pitch = XMConvertToRadians(localRot.x);
    float yaw   = XMConvertToRadians(localRot.y);
    float roll  = XMConvertToRadians(localRot.z);
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&quatLocalRotation_, q);

    isDirty_ = true;
}

void wbp_transform::TransformComponent::SetQuatRotation(const DirectX::XMFLOAT4 &quatRotation)
{
    previousRotation_ = quatRotation_;
    quatRotation_ = quatRotation;

    // If Previous is before initialization, set current value as previous value
    if (!previousQuatLocalRotInitialized_)
    {
        previousQuatLocalRotInitialized_ = true;
        previousRotation_ = quatRotation_;
    }
}

void wbp_transform::TransformComponent::SetLocalQuatRotation(const DirectX::XMFLOAT4 &localQuatRotation)
{
    quatLocalRotation_ = localQuatRotation;
    isDirty_ = true;
}

void wbp_transform::TransformComponent::SetScale(const DirectX::XMFLOAT3 &scale)
{
    previousLocalScale_ = scale_;
    scale_ = scale;

    // If Previous is before initialization, set current value as previous value
    if (!previousLocalScaleInitialized_)
    {
        previousLocalScaleInitialized_ = true;
        previousLocalScale_ = scale_;
    }
}

void wbp_transform::TransformComponent::SetLocalScale(const DirectX::XMFLOAT3 &localScale)
{
    localScale_ = localScale;
    isDirty_ = true;
}

void wbp_transform::TransformComponent::Translate(const XMFLOAT3 &translation)
{
    localPosition_.x += translation.x;
    localPosition_.y += translation.y;
    localPosition_.z += translation.z;

    isDirty_ = true;
}

void wbp_transform::TransformComponent::Rotate(const XMFLOAT3 &rotation)
{
    float pitch = XMConvertToRadians(rotation.x);
    float yaw   = XMConvertToRadians(rotation.y);
    float roll  = XMConvertToRadians(rotation.z);
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

    // Update quaternions in world
    XMVECTOR currentQuat = XMLoadFloat4(&quatLocalRotation_);
    XMVECTOR newQuat = XMQuaternionMultiply(q, currentQuat);
    XMStoreFloat4(&quatLocalRotation_, newQuat);

    // float outPitch, outYaw, outRoll;
    // XMQuaternionToEulerAngles(newQuat, &outPitch, &outYaw, &outRoll);

    // outRoll = 0.0f;

    // Euler angle → quaternion
    // XMVECTOR fixedQuat = XMQuaternionRotationRollPitchYaw(outPitch, outYaw, outRoll);
    // XMStoreFloat4(&quatLocalRotation_, fixedQuat);

    isDirty_ = true;
}

void wbp_transform::TransformComponent::Scale(const XMFLOAT3 &scale)
{
    localScale_.x *= scale.x;
    localScale_.y *= scale.y;
    localScale_.z *= scale.z;

    isDirty_ = true;
}

wb::IEntity *wbp_transform::TransformComponent::GetParent(wb::IEntityContainer &entityCont) const
{
    if (parentEntityID_) return &entityCont.Get(*parentEntityID_); // Parent was set
    return nullptr; // Parent was not set
}

void wbp_transform::TransformComponent::SetParent
(
    wb::IEntity *self, wb::IEntity *parent, 
    wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
){
    // If the parent Entity has already been set, remove itself from the parent Entity's child list
    if (parentEntityID_)
    {
        wb::IEntity &parentEntity = entityCont.Get(*parentEntityID_);

        wb::IComponent *component = parentEntity.GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::TransformComponent *parentTransform = wb::As<wbp_transform::TransformComponent>(component);
        if (!parentTransform)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Parent Entity does not have a TransformComponent."}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_TRANSFROM", err);
            wb::ThrowRuntimeError(err);
        }

        // Removes itself from the list of children of the parent Entity
        for (size_t i = 0; i < parentTransform->GetChildCount(); i++)
        {
            wb::IEntity *childEntity = parentTransform->GetChild(i, entityCont);
            if (childEntity->GetID()() == self->GetID()())
            {
                // parentTransform->childEntityIDs_.erase(childEntityIDs_.begin() + i);
                parentTransform->childEntityIDs_.erase(parentTransform->childEntityIDs_.begin() + i);
                break;
            }
        }
    }

    // If the new parent Entity is nullptr, clear the ID of the parent Entity
    if (!parent)
    {
        parentEntityID_ = nullptr;
        return;
    }   

    // Set ID of new parent Entity
    parentEntityID_ = parent->GetID().Clone();

    // Add itself to the list of children of the new parent
    {
        wb::IComponent *component = parent->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::TransformComponent *parentTransform = wb::As<wbp_transform::TransformComponent>(component);
        if (!parentTransform)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Parent Entity does not have a TransformComponent."}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_TRANSFROM", err);
            wb::ThrowRuntimeError(err);
        }

        // Adds itself to the list of children of the parent Entity
        parentTransform->childEntityIDs_.emplace_back(self->GetID().Clone());
    }
}

size_t wbp_transform::TransformComponent::GetChildCount() const
{
    return childEntityIDs_.size();
}

wb::IEntity *wbp_transform::TransformComponent::GetChild(const size_t &index, wb::IEntityContainer &entityCont) const
{
    // Obtains the ID of the child Entity at the specified index
    if (index >= childEntityIDs_.size())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Index out of bounds when accessing child Entity."}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_TRANSFROM", err);
        wb::ThrowRuntimeError(err);
    }

    if (!childEntityIDs_[index]->IsValid()) return nullptr;

    return &entityCont.Get(*childEntityIDs_[index]);
}

wb::IEntity &wbp_transform::TransformComponent::GetRoot
(
    wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
) const
{
    // Recursively traverse parent Entity to find root Entity
    wb::IEntity *currentEntity = GetParent(entityCont);
    while (currentEntity)
    {
        wb::IComponent *component = currentEntity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::TransformComponent *transformComponent = wb::As<wbp_transform::TransformComponent>(component);

        if (!transformComponent)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Current Entity does not have a TransformComponent."}
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_TRANSFROM", err);
            wb::ThrowRuntimeError(err);
        }

        currentEntity = transformComponent->GetParent(entityCont);
    }

    // If an Entity with no parent is found, it is the root Entity
    return *currentEntity;
}

XMMATRIX wbp_transform::TransformComponent::GetWorldMatrix() const
{
    XMMATRIX scaleMat = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
    XMVECTOR quat = XMLoadFloat4(&quatRotation_);
    XMMATRIX rotMat = XMMatrixRotationQuaternion(quat);
    XMMATRIX transMat = XMMatrixTranslation(position_.x, position_.y, position_.z);

    return scaleMat * rotMat * transMat;
}

DirectX::XMMATRIX wbp_transform::TransformComponent::GetWorldMatrixWithoutRot() const
{
    XMMATRIX scaleMat = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
    XMMATRIX transMat = XMMatrixTranslation(position_.x, position_.y, position_.z);
    
    return scaleMat * transMat;
}

DirectX::XMMATRIX wbp_transform::TransformComponent::GetPreviousWorldMatrix() const
{
    XMMATRIX scaleMat = XMMatrixScaling(previousLocalScale_.x, previousLocalScale_.y, previousLocalScale_.z);
    XMVECTOR quat = XMLoadFloat4(&previousRotation_);
    XMMATRIX rotMat = XMMatrixRotationQuaternion(quat);
    XMMATRIX transMat = XMMatrixTranslation(previousPosition_.x, previousPosition_.y, previousPosition_.z);

    return scaleMat * rotMat * transMat;
}

DirectX::XMMATRIX wbp_transform::TransformComponent::GetPreviousWorldMatrixWithoutRot() const
{
    XMMATRIX scaleMat = XMMatrixScaling(previousLocalScale_.x, previousLocalScale_.y, previousLocalScale_.z);
    XMMATRIX transMat = XMMatrixTranslation(previousPosition_.x, previousPosition_.y, previousPosition_.z);

    return scaleMat * transMat;
}

void wbp_transform::TransformComponent::UpdateDirtyFlags()
{
    if (isDirty_)
    {
        wasDirty_ = true;
        isDirty_ = false;
    }
    else
    {
        wasDirty_ = false;
    }
}

void wbp_transform::TransformComponent::CleanNotExistEntities
(
    wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
){
    // Clear parent Entity if parent ID is not valid
    if (parentEntityID_ && !parentEntityID_->IsValid())
    {
        parentEntityID_ = nullptr;
    }

    // If the ID of the child Entity is not valid, clear the ID of the child Entity
    size_t childCount = childEntityIDs_.size();
    for (size_t i = childCount; i-- > 0; )
    {
        if (childEntityIDs_[i] && !childEntityIDs_[i]->IsValid())
        {
            childEntityIDs_.erase(childEntityIDs_.begin() + i);
        }
    }
}

namespace wbp_transform
{
    WB_REGISTER_COMPONENT(TransformComponentID(), TransformComponent);

} // namespace wbp_transform