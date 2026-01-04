#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/component_collision_result.h"

namespace wbp_collision
{
    const WBP_COLLISION_API size_t &CollisionResultComponentID();

    class WBP_COLLISION_API CollisionResultComponent : public ICollisionResultComponent
    {
    private:
        std::vector<std::unique_ptr<wb::IOptionalValue>> collidedEntityIDs_;
        std::vector<bool> collidedIsTriggers_;

        std::vector<DirectX::XMFLOAT3> collidedFaceNormals_;

        std::vector<std::vector<DirectX::XMFLOAT3>> collidedPoints_;
        std::vector<std::vector<float>> collidedDistances_;

        std::unique_ptr<wb::IOptionalValue> minDistanceCollidedEntityID_ = nullptr;
        float minDistance_ = FLT_MAX;

        std::unique_ptr<wb::IOptionalValue> maxDistanceCollidedEntityID_ = nullptr;
        float maxDistance_ = -FLT_MAX;

    public:
        CollisionResultComponent() = default;
        ~CollisionResultComponent() override = default;

        CollisionResultComponent(const CollisionResultComponent &) = delete;
        CollisionResultComponent &operator=(const CollisionResultComponent &) = delete;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;

        /***************************************************************************************************************
         * ICollisionResultComponent implementation
        /**************************************************************************************************************/

        bool IsCollided() const override { return GetCollidedCount() > 0; }
        size_t GetCollidedCount() const override { return collidedEntityIDs_.size(); }

        const wb::IOptionalValue &GetCollidedEntityID(size_t index) const override;
        bool GetCollidedIsTrigger(size_t index) const override;

        const DirectX::XMFLOAT3 &GetCollidedFaceNormal(size_t index) const override;
        const std::vector<DirectX::XMFLOAT3> &GetCollidedPoints(size_t index) const override;
        const std::vector<float> &GetCollidedDistances(size_t index) const override;

        const wb::IOptionalValue *GetMinDistanceColliedEntityID() const override { return minDistanceCollidedEntityID_.get(); }
        const float &GetMinDistance() const override { return minDistance_; }

        const wb::IOptionalValue *GetMaxDistanceCollidedEntityID() const override { return maxDistanceCollidedEntityID_.get(); }
        const float &GetMaxDistance() const override { return maxDistance_; }

        void AddCollided
        (
            std::unique_ptr<wb::IOptionalValue> entityID, 
            const DirectX::XMFLOAT3 &normal, bool isTrigger
        ) override;

        void AddCollided
        (
            std::unique_ptr<wb::IOptionalValue> entityID, 
            std::vector<DirectX::XMFLOAT3> points, std::vector<float> distances,
            bool isTrigger
        ) override;

        void ClearCollided() override;
    };

} // namespace wbp_collision