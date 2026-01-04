#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/component_box_collider.h"

namespace wbp_collision
{
    const WBP_COLLISION_API size_t &BoxColliderComponentID();

    constexpr size_t DEFAULT_BOX_SHAPE_ASSET_ID = 0;

    class WBP_COLLISION_API BoxColliderComponent : public IBoxColliderComponent
    {
    private:
        bool collisionEnabled_ = DEFAULT_COLLISION_ENABLED;
        bool isTrigger_ = DEFAULT_IS_TRIGGER;
        bool canBeRunnerEnabled_ = true;
        bool isAlwaysRunnerEnabled_ = false;

        size_t colliderShapeAssetID_ = DEFAULT_BOX_SHAPE_ASSET_ID;

        std::vector<size_t> ignoreComponentIDs_;
        std::vector<size_t> targetComponentIDs_;

    public:
        BoxColliderComponent() = default;
        ~BoxColliderComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        virtual const size_t &GetID() const override;

        /***************************************************************************************************************
         * ICollider implementation
        /**************************************************************************************************************/

        void SetCollisionEnabled(bool enabled) override { collisionEnabled_ = enabled; }
        bool IsCollisionEnabled() const override { return collisionEnabled_; }

        void SetTrigger(bool isTrigger) override { isTrigger_ = isTrigger; }
        bool IsTrigger() const override { return isTrigger_; }

        void SetCanBeRunnerEnabled(bool enabled) override { canBeRunnerEnabled_ = enabled; }
        bool IsCanBeRunnerEnabled() const override { return canBeRunnerEnabled_; }

        void SetAlwaysRunnerEnabled(bool enabled) override { isAlwaysRunnerEnabled_ = enabled; }
        bool IsAlwaysRunnerEnabled() const override { return isAlwaysRunnerEnabled_; }

        /***************************************************************************************************************
         * IBoxColliderComponent implementation
        /**************************************************************************************************************/

        void SetColliderShapeAssetID(size_t colliderShapeAssetID) override { colliderShapeAssetID_ = colliderShapeAssetID; }

        size_t GetAABBCount(wb::IAssetContainer &assetContainer) const override;
        const std::vector<wbp_primitive::PrimitiveAABB> &GetAABBs(wb::IAssetContainer &assetContainer) const override;
        const wbp_primitive::PrimitiveAABB &GetAABB(size_t index, wb::IAssetContainer &assetContainer) const override;

        void AddIgnoreComponentID(size_t componentID) override;
        void ClearIgnoreComponentIDs() override { ignoreComponentIDs_.clear(); }
        const std::vector<size_t> &GetIgnoreComponentIDs() const override { return ignoreComponentIDs_; }

        void AddTargetComponentID(size_t componentID) override;
        void ClearTargetComponentIDs() override { targetComponentIDs_.clear(); }
        const std::vector<size_t> &GetTargetComponentIDs() const override { return targetComponentIDs_; }
    };

} // namespace wbp_collision