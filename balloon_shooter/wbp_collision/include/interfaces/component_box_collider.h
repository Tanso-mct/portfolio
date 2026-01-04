#pragma once
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/collider.h"

#include "wbp_primitive/include/primitive.h"
#pragma comment(lib, "wbp_primitive.lib")

#include <DirectXMath.h>

namespace wbp_collision
{
    class IBoxColliderComponent : public wb::IComponent, public ICollider
    {
    public:
        virtual ~IBoxColliderComponent() = default;

        // Set the ID of the Asset, which has the shape of a collider
        virtual void SetColliderShapeAssetID(size_t colliderShapeAssetID) = 0;

        virtual size_t GetAABBCount(wb::IAssetContainer &assetContainer) const = 0;
        virtual const std::vector<wbp_primitive::PrimitiveAABB> &GetAABBs(wb::IAssetContainer &assetContainer) const = 0;
        virtual const wbp_primitive::PrimitiveAABB &GetAABB(size_t index, wb::IAssetContainer &assetContainer) const = 0;

        virtual void AddIgnoreComponentID(size_t componentID) = 0;
        virtual void ClearIgnoreComponentIDs() = 0;
        virtual const std::vector<size_t> &GetIgnoreComponentIDs() const = 0;

        virtual void AddTargetComponentID(size_t componentID) = 0;
        virtual void ClearTargetComponentIDs() = 0;
        virtual const std::vector<size_t> &GetTargetComponentIDs() const = 0;
    };

} // namespace wbp_collision