#include "wbp_collision/src/pch.h"
#include "wbp_collision/include/component_box_collider.h"

#include "wbp_collision/include/interfaces/asset_collider_shape.h"

const WBP_COLLISION_API size_t &wbp_collision::BoxColliderComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_collision::BoxColliderComponent::GetID() const
{
    return BoxColliderComponentID();
}

size_t wbp_collision::BoxColliderComponent::GetAABBCount(wb::IAssetContainer &assetContainer) const
{
    wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(colliderShapeAssetID_);

    wbp_collision::IColliderShapeAsset *colliderShapeAsset = wb::As<wbp_collision::IColliderShapeAsset>(&asset());
    if (colliderShapeAsset == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "The asset with the ID does not implement or not collect IColliderShapeAsset.",
                "BoxColliderComponent requires IColliderShapeAsset to be set.",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }

    return colliderShapeAsset->GetAABBs().size();
}

const std::vector<wbp_primitive::PrimitiveAABB> &wbp_collision::BoxColliderComponent::GetAABBs
(
    wb::IAssetContainer &assetContainer
) const
{
    wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(colliderShapeAssetID_);

    wbp_collision::IColliderShapeAsset *colliderShapeAsset = wb::As<wbp_collision::IColliderShapeAsset>(&asset());
    if (colliderShapeAsset == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "The asset with the ID does not implement or not collect IColliderShapeAsset.",
                "BoxColliderComponent requires IColliderShapeAsset to be set.",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }

    return colliderShapeAsset->GetAABBs();
}

const wbp_primitive::PrimitiveAABB &wbp_collision::BoxColliderComponent::GetAABB
(
    size_t index, wb::IAssetContainer &assetContainer
) const
{
    wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(colliderShapeAssetID_);

    wbp_collision::IColliderShapeAsset *colliderShapeAsset = wb::As<wbp_collision::IColliderShapeAsset>(&asset());
    if (colliderShapeAsset == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "The asset with the ID does not implement or not collect IColliderShapeAsset.",
                "BoxColliderComponent requires IColliderShapeAsset to be set.",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WBP_COLLISION", err);
        wb::ThrowRuntimeError(err);
    }

    return colliderShapeAsset->GetAABBs().at(index);
}

void wbp_collision::BoxColliderComponent::AddIgnoreComponentID(size_t componentID)
{
    if (std::find(ignoreComponentIDs_.begin(), ignoreComponentIDs_.end(), componentID) == ignoreComponentIDs_.end())
    {
        ignoreComponentIDs_.push_back(componentID);
    }
}

void wbp_collision::BoxColliderComponent::AddTargetComponentID(size_t componentID)
{
    if (std::find(targetComponentIDs_.begin(), targetComponentIDs_.end(), componentID) == targetComponentIDs_.end())
    {
        targetComponentIDs_.push_back(componentID);
    }
}

namespace wbp_collision
{
    WB_REGISTER_COMPONENT(BoxColliderComponentID(), BoxColliderComponent);

} // namespace wbp_collision