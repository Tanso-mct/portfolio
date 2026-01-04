#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_physics/include/collider.h"
#include "mono_physics/include/shape.h"

#include <vector>

namespace mono_physics
{
    class MONO_PHYSICS_API RayCollisionResult : public CollisionResult
    {
    private:
        std::vector<DirectX::XMFLOAT3> collisionPoints_;
        std::vector<float> collisionDistances_;

        riaecs::Entity closestEntity_ = riaecs::Entity();
        DirectX::XMFLOAT3 closestPoint_ = { 0.0f, 0.0f, 0.0f };
        float closestDistance_ = FLT_MAX;

        riaecs::Entity furthestEntity_ = riaecs::Entity();
        DirectX::XMFLOAT3 furthestPoint_ = { 0.0f, 0.0f, 0.0f };
        float furthestDistance_ = 0.0f;

    public:
        RayCollisionResult();
        ~RayCollisionResult() override;

        virtual void Clear() override;

        const std::vector<DirectX::XMFLOAT3> &GetCollisionPoints() const { return collisionPoints_; }
        void AddCollisionPoint(const DirectX::XMFLOAT3 &point) { collisionPoints_.push_back(point); }
        void ClearCollisionPoints() { collisionPoints_.clear(); }

        const std::vector<float> &GetCollisionDistances() const { return collisionDistances_; }
        void AddCollisionDistance(float distance) { collisionDistances_.push_back(distance); }
        void ClearCollisionDistances() { collisionDistances_.clear(); }

        riaecs::Entity GetClosestEntity() const { return closestEntity_; }
        const DirectX::XMFLOAT3 &GetClosestPoint() const { return closestPoint_; }
        float GetClosestDistance() const { return closestDistance_; }
        void SetClosestEntity(riaecs::Entity entity, const DirectX::XMFLOAT3 &point, float distance);

        riaecs::Entity GetFurthestEntity() const { return furthestEntity_; }
        const DirectX::XMFLOAT3 &GetFurthestPoint() const { return furthestPoint_; }
        float GetFurthestDistance() const { return furthestDistance_; }
        void SetFurthestEntity(riaecs::Entity entity, const DirectX::XMFLOAT3 &point, float distance);
    };

    constexpr size_t ComponentRayColliderMaxCount = 1000;
    class MONO_PHYSICS_API ComponentRayCollider : public Collider
    {
    public:
        ComponentRayCollider();
        ~ComponentRayCollider();

        struct SetupParam
        {
            bool isTrigger = false;
            std::unique_ptr<CollisionResult> collisionResult = std::make_unique<RayCollisionResult>();
            std::unique_ptr<ShapeRay> ray = std::make_unique<ShapeRay>();
        };
        void Setup(SetupParam &param);

        const ShapeRay &GetRay() const;
        void SetRay(std::unique_ptr<ShapeRay> ray);

        const RayCollisionResult &GetRayCollisionResult() const;
        void SetRayCollisionResult(std::unique_ptr<RayCollisionResult> result);
    };

    extern MONO_PHYSICS_API riaecs::ComponentRegistrar
    <ComponentRayCollider, ComponentRayColliderMaxCount> ComponentRayColliderID;

} // namespace mono_physics