#include "mono_transform/src/pch.h"
#include "mono_transform/include/component_transform.h"

#include "mono_transform/include/xm_utils.h"

#pragma comment(lib, "riaecs.lib")

using namespace DirectX;

mono_transform::ComponentTransform::ComponentTransform()
{
}

mono_transform::ComponentTransform::~ComponentTransform()
{
    isInitialized_ = false;

    pos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    localPos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    lastPos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);

    rot_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    localRot_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    lastRot_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    scale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
    localScale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);
    lastScale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);

    parent_ = riaecs::Entity();
    childs_.clear();
}

void mono_transform::ComponentTransform::Setup(SetupParam &param)
{
    pos_ = param.pos_;
    lastPos_ = param.pos_;

    XMVECTOR qV = XMQuaternionRotationRollPitchYaw(param.pitch_, param.yaw_, param.roll_);
    XMFLOAT4 rot;
    XMStoreFloat4(&rot, qV);
    rot_ = rot;
    lastRot_ = rot;

    scale_ = param.scale_;
    lastScale_ = param.scale_;

    parent_ = riaecs::Entity();
    childs_ = std::vector<riaecs::Entity>();

    isInitialized_ = true;
}

void mono_transform::ComponentTransform::Setup(const SetupParamWithAnchor &param)
{
    XMVECTOR anchorSizeV = XMLoadFloat2(&param.anchor_.size_);
    XMVECTOR anchorPivotV = XMLoadFloat2(&param.anchor_.pivot_);
    XMVECTOR anchoredPosV = XMLoadFloat2(&param.anchoredPos_);
    XMVECTOR anchoredSizeV = XMLoadFloat2(&param.anchoredSize_);

    // Position
    XMVECTOR posV = (anchorPivotV * anchorSizeV + anchoredPosV) / anchorSizeV; // 0.0 ~ 1.0
    posV = posV * 2.0f - XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f); // -1.0 ~ 1.0
    posV = XMVectorSetZ(posV, param.depth_); // NDC space
    XMStoreFloat3(&pos_, posV);
    XMStoreFloat3(&lastPos_, posV);

    // Rotation
    XMVECTOR qV = XMQuaternionRotationRollPitchYaw(param.pitch_, param.yaw_, param.roll_);
    XMStoreFloat4(&rot_, qV);
    XMStoreFloat4(&lastRot_, qV);

    // ScaleRW
    XMVECTOR scaleV = anchoredSizeV / anchorSizeV;
    scaleV = XMVectorSetZ(scaleV, 1.0f);
    XMStoreFloat3(&scale_, scaleV);
    XMStoreFloat3(&lastScale_, scaleV);

    parent_ = riaecs::Entity();
    childs_ = std::vector<riaecs::Entity>();

    isInitialized_ = true;
}

void mono_transform::ComponentTransform::SetPos(const DirectX::XMFLOAT3 &pos, riaecs::IECSWorld &ecsWorld)
{
    pos_ = pos;

    // If has parent, update local position
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        localPos_ = mono_transform::CreateNewLocalPosFromWorldPos(pos_, localPos_, parentTransform->GetPos());
    }
}

void mono_transform::ComponentTransform::SetRotFromQuat(const DirectX::XMFLOAT4 &rot, riaecs::IECSWorld &ecsWorld)
{
    rot_ = rot;

    // If has parent, update local rotation
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        localRot_ = mono_transform::CreateNewLocalRotFromWorldRot(rot_, localRot_, parentTransform->GetRotByQuat());
    }
}

DirectX::XMFLOAT3 mono_transform::ComponentTransform::GetRotByEuler() const
{
    XMVECTOR q = XMLoadFloat4(&rot_);
    float pitch, yaw, roll;
    mono_transform::XMQuaternionToEulerAngles(q, pitch, yaw, roll);

    // Convert radians to degrees
    XMFLOAT3 euler(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), XMConvertToDegrees(roll));
    return euler;
}

void mono_transform::ComponentTransform::SetRotFromEuler(
    float pitch, float yaw, float roll, riaecs::IECSWorld &ecsWorld)
{
    // Convert degrees to radians
    pitch = XMConvertToRadians(pitch);
    yaw = XMConvertToRadians(yaw);
    roll = XMConvertToRadians(roll);

    // Create quaternion from Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&rot_, q);

    // If has parent, update local rotation
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        localRot_ = mono_transform::CreateNewLocalRotFromWorldRot(rot_, localRot_, parentTransform->GetRotByQuat());
    }
}

void mono_transform::ComponentTransform::SetScale(
    const DirectX::XMFLOAT3 &scale, riaecs::IECSWorld &ecsWorld)
{
    scale_ = scale;

    // If has parent, update local scale
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        localScale_ = mono_transform::CreateNewLocalScaleFromWorldScale(scale_, localScale_, parentTransform->GetScale());
    }
}

void mono_transform::ComponentTransform::SetLocalPos(
    const DirectX::XMFLOAT3 &localPos, riaecs::IECSWorld &ecsWorld)
{
    localPos_ = localPos;

    // If has parent, update world position
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        pos_ = mono_transform::CreateNewWorldPosFromLocalPos(localPos_, pos_, parentTransform->GetPos());
    }
}

void mono_transform::ComponentTransform::SetLocalRotFromQuat(
    const DirectX::XMFLOAT4 &localRot, riaecs::IECSWorld &ecsWorld)
{
    localRot_ = localRot;

    // If has parent, update world rotation
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        rot_ = mono_transform::CreateNewWorldRotFromLocalRot(localRot_, rot_, parentTransform->GetRotByQuat());
    }
}

DirectX::XMFLOAT3 mono_transform::ComponentTransform::GetLocalRotByEuler() const
{
    XMVECTOR q = XMLoadFloat4(&localRot_);
    float pitch, yaw, roll;
    mono_transform::XMQuaternionToEulerAngles(q, pitch, yaw, roll);

    // Convert radians to degrees
    XMFLOAT3 euler(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), XMConvertToDegrees(roll));
    return euler;
}

void mono_transform::ComponentTransform::SetLocalRotFromEuler(
    float pitch, float yaw, float roll, riaecs::IECSWorld &ecsWorld)
{
    // Convert degrees to radians
    pitch = XMConvertToRadians(pitch);
    yaw = XMConvertToRadians(yaw);
    roll = XMConvertToRadians(roll);

    // Create quaternion from Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&localRot_, q);

    // If has parent, update world rotation
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        rot_ = mono_transform::CreateNewWorldRotFromLocalRot(localRot_, rot_, parentTransform->GetRotByQuat());
    }
}

void mono_transform::ComponentTransform::SetLocalScale(
    const DirectX::XMFLOAT3 &localScale, riaecs::IECSWorld &ecsWorld)
{
    localScale_ = localScale;

    // If has parent, update world scale
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        scale_ = mono_transform::CreateNewWorldScaleFromLocalScale(localScale_, scale_, parentTransform->GetScale());
    }
}

DirectX::XMFLOAT3 mono_transform::ComponentTransform::GetLastRotByEuler() const
{
    XMVECTOR q = XMLoadFloat4(&lastRot_);
    float pitch, yaw, roll;
    mono_transform::XMQuaternionToEulerAngles(q, pitch, yaw, roll);

    // Convert radians to degrees
    XMFLOAT3 euler(XMConvertToDegrees(pitch), XMConvertToDegrees(yaw), XMConvertToDegrees(roll));
    return euler;
}

void mono_transform::ComponentTransform::SetLastRotFromEuler(float pitch, float yaw, float roll)
{
    // Convert degrees to radians
    pitch = XMConvertToRadians(pitch);
    yaw = XMConvertToRadians(yaw);
    roll = XMConvertToRadians(roll);

    // Create quaternion from Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&lastRot_, q);
}

void mono_transform::ComponentTransform::SetPosWithAnchor(
    const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredPos, float depth, riaecs::IECSWorld &ecsWorld)
{
    XMVECTOR anchorSizeVec = XMLoadFloat2(&anchor.size_);
    XMVECTOR anchorPivotV = XMLoadFloat2(&anchor.pivot_);

    XMVECTOR anchoredPosV = XMLoadFloat2(&anchoredPos);
    XMVECTOR posNDC = (anchorPivotV * anchorSizeVec + anchoredPosV) / anchorSizeVec; // 0.0 ~ 1.0
    posNDC = posNDC * 2.0f - XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f); // -1.0 ~ 1.0
    posNDC = XMVectorSetZ(posNDC, depth); // NDC space
    XMStoreFloat3(&pos_, posNDC);

    // If has parent, update local position
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        XMMATRIX newLocalMat = XMMatrixInverse(nullptr, parentTransform->GetWorldMatrix()) * GetWorldMatrix();
        XMVECTOR dePosV, deRotV, deScaleV;
        XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, newLocalMat);

        XMStoreFloat3(&localPos_, dePosV);
    }
}

void mono_transform::ComponentTransform::SetLocalPosWithAnchor(
    const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredLocalPos, float depth, riaecs::IECSWorld &ecsWorld)
{
    XMVECTOR anchorSizeVec = XMLoadFloat2(&anchor.size_);
    XMVECTOR anchorPivotV = XMLoadFloat2(&anchor.pivot_);

    XMVECTOR anchoredPosV = XMLoadFloat2(&anchoredLocalPos);
    XMVECTOR posNDC = (anchorPivotV * anchorSizeVec + anchoredPosV) / anchorSizeVec;
    posNDC = posNDC * 2.0f - XMVectorSet(1.0f, 1.0f, 0.0f, 0.0f); // -1.0 ~ 1.0
    posNDC = XMVectorSetZ(posNDC, depth);
    XMStoreFloat3(&localPos_, posNDC);

    // If has parent, update world position
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        XMMATRIX newWorldMat = parentTransform->GetWorldMatrix() * GetLocalMatrix();
        XMVECTOR dePosV, deRotV, deScaleV;
        XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, newWorldMat);

        XMStoreFloat3(&pos_, dePosV);
    }
}

void mono_transform::ComponentTransform::SetScaleWithAnchor(
    const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredSize, riaecs::IECSWorld &ecsWorld)
{
    XMVECTOR anchorSizeVec = XMLoadFloat2(&anchor.size_);
    XMVECTOR anchoredSizeV = XMLoadFloat2(&anchoredSize);
    XMVECTOR scale = anchoredSizeV / anchorSizeVec;
    scale = XMVectorSetZ(scale, 1.0f);
    XMStoreFloat3(&scale_, scale);

    // If has parent, update local scale
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        XMMATRIX newLocalMat = XMMatrixInverse(nullptr, parentTransform->GetWorldMatrix()) * GetWorldMatrix();
        XMVECTOR dePosV, deRotV, deScaleV;
        XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, newLocalMat);

        XMStoreFloat3(&localScale_, deScaleV);
    }
}

void mono_transform::ComponentTransform::SetLocalScaleWithAnchor(
    const Anchor &anchor, const DirectX::XMFLOAT2 &anchoredLocalSize, riaecs::IECSWorld &ecsWorld)
{
    XMVECTOR anchorSizeVec = XMLoadFloat2(&anchor.size_);
    XMVECTOR anchoredSizeV = XMLoadFloat2(&anchoredLocalSize);
    XMVECTOR scale = anchoredSizeV / anchorSizeVec;
    scale = XMVectorSetZ(scale, 1.0f);
    XMStoreFloat3(&localScale_, scale);

    // If has parent, update world scale
    if (parent_.IsValid() && ecsWorld.CheckEntityExist(parent_))
    {
        ComponentTransform* parentTransform 
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        XMMATRIX newWorldMat = parentTransform->GetWorldMatrix() * GetLocalMatrix();
        XMVECTOR dePosV, deRotV, deScaleV;
        XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, newWorldMat);

        XMStoreFloat3(&scale_, deScaleV);
    }
}

void mono_transform::ComponentTransform::SetParent(
    const riaecs::Entity &self, const riaecs::Entity &parent, riaecs::IECSWorld &ecsWorld)
{
    // Get self transform
    mono_transform::ComponentTransform* selfTransform
    = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
        ecsWorld, self, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    // Get parent transform
    mono_transform::ComponentTransform* newParentTransform
    = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
        ecsWorld, parent, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    // Removes itself from the current parent's childs
    if (selfTransform->parent_.IsValid() && ecsWorld.CheckEntityExist(selfTransform->parent_))
    {
        mono_transform::ComponentTransform* currentParentTransform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, selfTransform->parent_, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        selfTransform->RemoveChild(self, self, ecsWorld); // Remove itself from current parent's child list
    }

    // Set new parent
    selfTransform->parent_ = parent;

    // Add itself to the new parent's childs
    newParentTransform->childs_.push_back(self);

    // Update local transform to maintain world transform
    XMMATRIX newLocalMatrix = XMMatrixInverse(nullptr, newParentTransform->GetWorldMatrix()) * selfTransform->GetWorldMatrix();
    XMVECTOR dePosV, deRotV, deScaleV;
    XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, newLocalMatrix);

    XMStoreFloat3(&selfTransform->localPos_, dePosV);
    XMStoreFloat4(&selfTransform->localRot_, deRotV);
    XMStoreFloat3(&selfTransform->localScale_, deScaleV);
}

void mono_transform::ComponentTransform::RemoveChild(
    const riaecs::Entity &self, const riaecs::Entity &child, riaecs::IECSWorld &ecsWorld)
{
    // Get self transform
    mono_transform::ComponentTransform* selfTransform
    = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
        ecsWorld, self, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    auto it = std::find(selfTransform->childs_.begin(), selfTransform->childs_.end(), child);
    if (!(it != selfTransform->childs_.end() && ecsWorld.CheckEntityExist(child)))
        return; // Child not found or does not exist

    // Remove from child list
    selfTransform->childs_.erase(it);

    // Get child transform
    mono_transform::ComponentTransform* childTransform
    = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
        ecsWorld, child, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    // Update child's local transform to maintain world transform
    XMMATRIX childWorldMat = selfTransform->GetWorldMatrix() * childTransform->GetLocalMatrix();

    // Decompose world matrix to position, rotation, scale
    XMVECTOR dePosV, deRotV, deScaleV;
    XMMatrixDecompose(&deScaleV, &deRotV, &dePosV, childWorldMat);
    XMStoreFloat3(&childTransform->pos_, dePosV);
    XMStoreFloat4(&childTransform->rot_, deRotV);
    XMStoreFloat3(&childTransform->scale_, deScaleV);

    // Now that the child has no parent, set local = 0
    childTransform->localPos_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    childTransform->localRot_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    childTransform->localScale_ = XMFLOAT3(1.0f, 1.0f, 1.0f);

    // Remove parent reference from child
    childTransform->parent_ = riaecs::Entity();
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetWorldMatrix()
{
    XMMATRIX scale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&rot_));
    XMMATRIX pos = XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

    return scale * rot * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetWorldMatrixNoRot()
{
    XMMATRIX scale = XMMatrixScaling(scale_.x, scale_.y, scale_.z);
    XMMATRIX pos = XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

    return scale * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetWorldMatrixNoScale()
{
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&rot_));
    XMMATRIX pos = XMMatrixTranslation(pos_.x, pos_.y, pos_.z);

    return rot * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLocalMatrix()
{
    XMMATRIX scale = XMMatrixScaling(localScale_.x, localScale_.y, localScale_.z);
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&localRot_));
    XMMATRIX pos = XMMatrixTranslation(localPos_.x, localPos_.y, localPos_.z);

    return scale * rot * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLocalMatrixNoRot()
{
    XMMATRIX scale = XMMatrixScaling(localScale_.x, localScale_.y, localScale_.z);
    XMMATRIX pos = XMMatrixTranslation(localPos_.x, localPos_.y, localPos_.z);

    return scale * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLocalMatrixNoScale()
{
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&localRot_));
    XMMATRIX pos = XMMatrixTranslation(localPos_.x, localPos_.y, localPos_.z);

    return rot * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLastWorldMatrix()
{
    XMMATRIX scale = XMMatrixScaling(lastScale_.x, lastScale_.y, lastScale_.z);
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&lastRot_));
    XMMATRIX pos = XMMatrixTranslation(lastPos_.x, lastPos_.y, lastPos_.z);

    return scale * rot * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLastWorldMatrixNoRot()
{
    XMMATRIX scale = XMMatrixScaling(lastScale_.x, lastScale_.y, lastScale_.z);
    XMMATRIX pos = XMMatrixTranslation(lastPos_.x, lastPos_.y, lastPos_.z);

    return scale * pos;
}

DirectX::XMMATRIX mono_transform::ComponentTransform::GetLastWorldMatrixNoScale()
{
    XMMATRIX rot = XMMatrixRotationQuaternion(XMLoadFloat4(&lastRot_));
    XMMATRIX pos = XMMatrixTranslation(lastPos_.x, lastPos_.y, lastPos_.z);

    return rot * pos;
}

MONO_TRANSFORM_API riaecs::ComponentRegistrar
<mono_transform::ComponentTransform, mono_transform::ComponentTransformMaxCount> mono_transform::ComponentTransformID;

MONO_TRANSFORM_API DirectX::XMFLOAT3 mono_transform::CreateNewLocalPosFromWorldPos(
    const DirectX::XMFLOAT3 &worldPos, const DirectX::XMFLOAT3 &oldLocalPos, const DirectX::XMFLOAT3 &parentWorldPos)
{
    // Convert to XMVECTOR
    XMVECTOR worldPosV = XMLoadFloat3(&worldPos);
    XMVECTOR oldLocalPosV = XMLoadFloat3(&oldLocalPos);
    XMVECTOR parentWorldPosV = XMLoadFloat3(&parentWorldPos);

    // Calculate old world position from old local position
    XMVECTOR oldWorldPosV = parentWorldPosV + oldLocalPosV;

    // Calculate the difference between new world position and old world position
    XMVECTOR deltaV = worldPosV - oldWorldPosV;

    // Calculate new local position
    XMVECTOR newLocalPosV = oldLocalPosV + deltaV;

    // Convert back to XMFLOAT3
    XMFLOAT3 newLocalPos;
    XMStoreFloat3(&newLocalPos, newLocalPosV);

    return newLocalPos;
}

MONO_TRANSFORM_API DirectX::XMFLOAT3 mono_transform::CreateNewWorldPosFromLocalPos(
    const DirectX::XMFLOAT3 &localPos, const DirectX::XMFLOAT3 &oldWorldPos, const DirectX::XMFLOAT3 &parentWorldPos)
{
    // Convert to XMVECTOR
    XMVECTOR localPosV = XMLoadFloat3(&localPos);
    XMVECTOR oldWorldPosV = XMLoadFloat3(&oldWorldPos);
    XMVECTOR parentWorldPosV = XMLoadFloat3(&parentWorldPos);

    // Calculate old local position from old world position
    XMVECTOR oldLocalPosV = oldWorldPosV - parentWorldPosV;

    // Calculate the difference between new local position and old local position
    XMVECTOR deltaV = localPosV - oldLocalPosV;

    // Calculate new world position
    XMVECTOR newWorldPosV = oldWorldPosV + deltaV;

    // Convert back to XMFLOAT3
    XMFLOAT3 newWorldPos;
    XMStoreFloat3(&newWorldPos, newWorldPosV);

    return newWorldPos;
}

MONO_TRANSFORM_API DirectX::XMFLOAT4 mono_transform::CreateNewLocalRotFromWorldRot(
    const DirectX::XMFLOAT4 &worldRot, const DirectX::XMFLOAT4 &oldLocalRot, const DirectX::XMFLOAT4 &parentWorldRot)
{
    // Convert to XMVECTOR
    XMVECTOR worldRotV = XMLoadFloat4(&worldRot);
    XMVECTOR oldLocalRotV = XMLoadFloat4(&oldLocalRot);
    XMVECTOR parentWorldRotV = XMLoadFloat4(&parentWorldRot);

    // Calculate old world rotation from old local rotation
    XMVECTOR oldWorldRotV = XMQuaternionMultiply(oldLocalRotV, parentWorldRotV);

    // Calculate the difference between new world rotation and old world rotation
    XMVECTOR deltaV = XMQuaternionMultiply(worldRotV, XMQuaternionInverse(oldWorldRotV));

    // Calculate new local rotation
    XMVECTOR newLocalRotV = XMQuaternionMultiply(deltaV, oldLocalRotV);

    // Convert back to XMFLOAT4
    XMFLOAT4 newLocalRot;
    XMStoreFloat4(&newLocalRot, newLocalRotV);

    return newLocalRot;
}

MONO_TRANSFORM_API DirectX::XMFLOAT4 mono_transform::CreateNewWorldRotFromLocalRot(
    const DirectX::XMFLOAT4 &localRot, const DirectX::XMFLOAT4 &oldWorldRot, const DirectX::XMFLOAT4 &parentWorldRot)
{
    // Convert to XMVECTOR
    XMVECTOR localRotV = XMLoadFloat4(&localRot);
    XMVECTOR oldWorldRotV = XMLoadFloat4(&oldWorldRot);
    XMVECTOR parentWorldRotV = XMLoadFloat4(&parentWorldRot);

    // Calculate old local rotation from old world rotation
    XMVECTOR oldLocalRotV = XMQuaternionMultiply(oldWorldRotV, XMQuaternionInverse(parentWorldRotV));

    // Calculate the difference between new local rotation and old local rotation
    XMVECTOR deltaV = XMQuaternionMultiply(localRotV, XMQuaternionInverse(oldLocalRotV));

    // Calculate new world rotation
    XMVECTOR newWorldRotV = XMQuaternionMultiply(deltaV, oldWorldRotV);

    // Convert back to XMFLOAT4
    XMFLOAT4 newWorldRot;
    XMStoreFloat4(&newWorldRot, newWorldRotV);

    return newWorldRot;
}

MONO_TRANSFORM_API DirectX::XMFLOAT3 mono_transform::CreateNewLocalScaleFromWorldScale(
    const DirectX::XMFLOAT3 &worldScale, const DirectX::XMFLOAT3 &oldLocalScale, const DirectX::XMFLOAT3 &parentWorldScale)
{
    // Convert to XMVECTOR
    XMVECTOR worldScaleV = XMLoadFloat3(&worldScale);
    XMVECTOR oldLocalScaleV = XMLoadFloat3(&oldLocalScale);
    XMVECTOR parentWorldScaleV = XMLoadFloat3(&parentWorldScale);

    // Calculate old world scale from old local scale
    XMVECTOR oldWorldScaleV = oldLocalScaleV * parentWorldScaleV;

    // Calculate the difference between new world scale and old world scale
    XMVECTOR deltaV = worldScaleV / oldWorldScaleV;

    // Calculate new local scale
    XMVECTOR newLocalScaleV = oldLocalScaleV * deltaV;

    // Convert back to XMFLOAT3
    XMFLOAT3 newLocalScale;
    XMStoreFloat3(&newLocalScale, newLocalScaleV);

    return newLocalScale;
}

MONO_TRANSFORM_API DirectX::XMFLOAT3 mono_transform::CreateNewWorldScaleFromLocalScale(
    const DirectX::XMFLOAT3 &localScale, const DirectX::XMFLOAT3 &oldWorldScale, const DirectX::XMFLOAT3 &parentWorldScale)
{
    // Convert to XMVECTOR
    XMVECTOR localScaleV = XMLoadFloat3(&localScale);
    XMVECTOR oldWorldScaleV = XMLoadFloat3(&oldWorldScale);
    XMVECTOR parentWorldScaleV = XMLoadFloat3(&parentWorldScale);

    // Calculate old local scale from old world scale
    XMVECTOR oldLocalScaleV = oldWorldScaleV / parentWorldScaleV;

    // Calculate the difference between new local scale and old local scale
    XMVECTOR deltaV = localScaleV / oldLocalScaleV;

    // Calculate new world scale
    XMVECTOR newWorldScaleV = oldWorldScaleV * deltaV;

    // Convert back to XMFLOAT3
    XMFLOAT3 newWorldScale;
    XMStoreFloat3(&newWorldScale, newWorldScaleV);

    return newWorldScale;
}

MONO_TRANSFORM_API void mono_transform::UpdateRootTransform(ComponentTransform* component)
{
    // Store last transform
    component->SetLastPos(component->GetPos());
    component->SetLastRotFromQuat(component->GetRotByQuat());
    component->SetLastScale(component->GetScale());
}

MONO_TRANSFORM_API void mono_transform::UpdateChildTransform(
    ComponentTransform *self, ComponentTransform *parent, riaecs::IECSWorld &ecsWorld)
{
    // Store last transform
    self->SetLastPos(self->GetPos());
    self->SetLastRotFromQuat(self->GetRotByQuat());
    self->SetLastScale(self->GetScale());

    // Store parent's transform to vectors
    XMVECTOR parentPosV = XMLoadFloat3(&parent->GetPos());
    XMVECTOR parentRotV = XMLoadFloat4(&parent->GetRotByQuat());
    XMVECTOR parentScaleV = XMLoadFloat3(&parent->GetScale());

    // Store self's local transform to vectors
    XMVECTOR localPosV = XMLoadFloat3(&self->GetLocalPos());
    XMVECTOR localRotV = XMLoadFloat4(&self->GetLocalRotByQuat());
    XMVECTOR localScaleV = XMLoadFloat3(&self->GetLocalScale());

    // Calculate world position
    XMVECTOR worldPosV = parentPosV + XMVector3Rotate(localPosV * parentScaleV, parentRotV);
    XMFLOAT3 worldPos;
    XMStoreFloat3(&worldPos, worldPosV);
    self->SetPos(worldPos, ecsWorld);

    // Calculate world rotation
    XMVECTOR worldRotV = XMQuaternionMultiply(localRotV, parentRotV);
    XMFLOAT4 worldRot;
    XMStoreFloat4(&worldRot, worldRotV);
    self->SetRotFromQuat(worldRot, ecsWorld);

    // Calculate world scale
    XMVECTOR worldScaleV = localScaleV * parentScaleV;
    XMFLOAT3 worldScale;
    XMStoreFloat3(&worldScale, worldScaleV);
    self->SetScale(worldScale, ecsWorld);
}

MONO_TRANSFORM_API void mono_transform::UpdateChildsTransform(const riaecs::Entity &entity, riaecs::IECSWorld &ecsWorld)
{
    // Get transform
    ComponentTransform* transform
    = riaecs::GetComponentWithCheck<ComponentTransform>(
        ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    // If has childs, update them
    if (transform->GetChilds().empty())
        return;

    for (const riaecs::Entity &child : transform->GetChilds())
    {
        if (!ecsWorld.CheckEntityExist(child))
            continue;

        ComponentTransform* childTransform
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, child, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        UpdateChildTransform(childTransform, transform, ecsWorld);
        UpdateChildsTransform(child, ecsWorld); // Recursively update
    }
}

MONO_TRANSFORM_API void mono_transform::UpdateRootTransformNoLastTransform(ComponentTransform *component)
{
    
}

MONO_TRANSFORM_API void mono_transform::UpdateChildTransformNoLastTransform(
    ComponentTransform *self, ComponentTransform *parent, riaecs::IECSWorld &ecsWorld)
{
    // Store parent's transform to vectors
    XMVECTOR parentPosV = XMLoadFloat3(&parent->GetPos());
    XMVECTOR parentRotV = XMLoadFloat4(&parent->GetRotByQuat());
    XMVECTOR parentScaleV = XMLoadFloat3(&parent->GetScale());

    // Store self's local transform to vectors
    XMVECTOR localPosV = XMLoadFloat3(&self->GetLocalPos());
    XMVECTOR localRotV = XMLoadFloat4(&self->GetLocalRotByQuat());
    XMVECTOR localScaleV = XMLoadFloat3(&self->GetLocalScale());

    // Calculate world position
    XMVECTOR worldPosV = parentPosV + XMVector3Rotate(localPosV * parentScaleV, parentRotV);
    XMFLOAT3 worldPos;
    XMStoreFloat3(&worldPos, worldPosV);
    self->SetPos(worldPos, ecsWorld);

    // Calculate world rotation
    XMVECTOR worldRotV = XMQuaternionMultiply(localRotV, parentRotV);
    XMFLOAT4 worldRot;
    XMStoreFloat4(&worldRot, worldRotV);
    self->SetRotFromQuat(worldRot, ecsWorld);

    // Calculate world scale
    XMVECTOR worldScaleV = localScaleV * parentScaleV;
    XMFLOAT3 worldScale;
    XMStoreFloat3(&worldScale, worldScaleV);
    self->SetScale(worldScale, ecsWorld);
}

MONO_TRANSFORM_API void mono_transform::UpdateChildsTransformNoLastTransform(
    const riaecs::Entity &entity, riaecs::IECSWorld &ecsWorld)
{
    // Get transform
    ComponentTransform* transform
    = riaecs::GetComponentWithCheck<ComponentTransform>(
        ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

    // If has childs, update them
    if (transform->GetChilds().empty())
        return;

    for (const riaecs::Entity &child : transform->GetChilds())
    {
        if (!ecsWorld.CheckEntityExist(child))
            continue;

        ComponentTransform* childTransform
        = riaecs::GetComponentWithCheck<ComponentTransform>(
            ecsWorld, child, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        UpdateChildTransformNoLastTransform(childTransform, transform, ecsWorld);
        UpdateChildsTransformNoLastTransform(child, ecsWorld); // Recursively update
    }
}