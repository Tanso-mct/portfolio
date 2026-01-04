#include "wbp_transform/src/pch.h"
#include "wbp_transform/include/system_transform.h"

#include "wbp_transform/include/component_transform.h"

#include <DirectXMath.h>
using namespace DirectX;

const WBP_TRANSFORM_API size_t &wbp_transform::TransformSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &wbp_transform::TransformSystem::GetID() const
{
    return TransformSystemID();
}

namespace
{
    void QuaternionToYawPitchRoll(const XMVECTOR& q, float& yaw, float& pitch, float& roll) 
    {
        float x = XMVectorGetX(q);
        float y = XMVectorGetY(q);
        float z = XMVectorGetZ(q);
        float w = XMVectorGetW(q);

        // Pitch
        float sinp = 2.0f * (w * x + y * z);
        float cosp = 1.0f - 2.0f * (x * x + y * y);
        pitch = std::atan2(sinp, cosp);

        // Yaw
        float siny = 2.0f * (w * y - z * x);
        if (std::abs(siny) >= 1) yaw = std::copysign(XM_PIDIV2, siny); // 90
        else yaw = std::asin(siny);

        // Roll
        float sinr = 2.0f * (w * z + x * y);
        float cosr = 1.0f - 2.0f * (y * y + z * z);
        roll = std::atan2(sinr, cosr);
    }

}

void wbp_transform::TransformSystem::Initialize(wb::IAssetContainer &assetCont)
{
}

void wbp_transform::TransformSystem::Update(const wb::SystemArgument &args)
{
    // Cleanup parent-child relationships in non-existent Entities
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_transform::TransformComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        transform->CleanNotExistEntities(args.entityContainer_, args.componentContainer_);
    }

    // Update value by parent-child relationships
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(wbp_transform::TransformComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);

        // Skip if the Entity is not a root Entity
        if (transform->GetParent(args.entityContainer_) != nullptr) continue;

        UpdateRootTransform(entity, args.entityContainer_, args.componentContainer_);
        UpdateChildTransform(entity, args.entityContainer_, args.componentContainer_);
    }
}

void wbp_transform::TransformSystem::UpdateRootTransform
(
    wb::IEntity *root, 
    wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
){
    wb::IComponent *rootComponent = root->GetComponent(wbp_transform::TransformComponentID(), componentCont);
    wbp_transform::ITransformComponent *rootTransform = wb::As<wbp_transform::ITransformComponent>(rootComponent);

    rootTransform->SetPosition(rootTransform->GetLocalPosition());
    rootTransform->SetQuatRotation(rootTransform->GetQuatLocalRotation());
    rootTransform->SetScale(rootTransform->GetLocalScale());
}

void wbp_transform::TransformSystem::UpdateChildTransform
(
    wb::IEntity *parent, 
    wb::IEntityContainer &entityCont, wb::IComponentContainer &componentCont
){
    wb::IComponent *parentComponent = parent->GetComponent(wbp_transform::TransformComponentID(), componentCont);
    wbp_transform::ITransformComponent *parentTransform = wb::As<wbp_transform::ITransformComponent>(parentComponent);

    // Loop over the number of child Entities
    for (size_t i = 0; i < parentTransform->GetChildCount(); ++i)
    {
        wb::IEntity *childEntity = parentTransform->GetChild(i, entityCont);

        wb::IComponent *childComponent = childEntity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *childTransform = wb::As<wbp_transform::ITransformComponent>(childComponent);

        XMVECTOR parentWorldQuat = XMLoadFloat4(&parentTransform->GetQuatRotation());
        XMVECTOR childLocalQuat = XMLoadFloat4(&childTransform->GetQuatLocalRotation());

        // Calculate the world rotation of the child
        XMVECTOR childWorldQuat = XMQuaternionMultiply(childLocalQuat, parentWorldQuat);
        XMFLOAT4 childWorldQuatFloat;
        XMStoreFloat4(&childWorldQuatFloat, childWorldQuat);

        // Set the child's world quaternion
        childTransform->SetQuatRotation(childWorldQuatFloat);

        // Apply local rotation of the child
        XMVECTOR childLocalPosVec = XMLoadFloat3(&childTransform->GetLocalPosition());
        XMVECTOR rotatedByChild = XMVector3Rotate(childLocalPosVec, childLocalQuat);

        // Apply parent's world rotation
        XMVECTOR rotatedByParent = XMVector3Rotate(rotatedByChild, parentWorldQuat);

        // Add parent's world coordinates
        XMVECTOR parentWorldPosVec = XMLoadFloat3(&parentTransform->GetPosition());
        XMVECTOR childWorldPosVec = XMVectorAdd(parentWorldPosVec, rotatedByParent);

        // Write back to world
        XMFLOAT3 childWorldPos;
        XMStoreFloat3(&childWorldPos, childWorldPosVec);
        childTransform->SetPosition(childWorldPos);

        // Apply local scale of the child
        XMVECTOR childLocalScaleVec = XMLoadFloat3(&childTransform->GetLocalScale());
        XMVECTOR parentWorldScaleVec = XMLoadFloat3(&parentTransform->GetScale());
        XMVECTOR childWorldScaleVec = XMVectorMultiply(childLocalScaleVec, parentWorldScaleVec);

        // Write back to world scale
        XMFLOAT3 childWorldScale;
        XMStoreFloat3(&childWorldScale, childWorldScaleVec);
        childTransform->SetScale(childWorldScale);

        // If the parent is Dirty, the child should also be Dirty
        if (parentTransform->IsDirty())
        {
            childTransform->SetDirty(true);
        }

        UpdateChildTransform(childEntity, entityCont, componentCont);
    }

    parentTransform->UpdateDirtyFlags();
}

namespace wbp_transform
{
    WB_REGISTER_SYSTEM(TransformSystem, TransformSystemID());

} // namespace wbp_transform