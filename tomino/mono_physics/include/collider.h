#pragma once
#include "mono_physics/include/dll_config.h"

#include "mono_physics/include/collision_result.h"
#include "mono_physics/include/shape.h"

#include <vector>
#include <memory>

namespace mono_physics
{
    class MONO_PHYSICS_API Collider
    {
    protected:
        std::unique_ptr<IShape> shape_ = nullptr;
        bool isTrigger_ = false;
        bool collisionEnabled_ = true;

        std::unique_ptr<CollisionResult> collisionResult_ = nullptr;
        std::vector<size_t> collidableComponentIDs_;
        ShapeBox boundingBox_ = ShapeBox();

    public:
        Collider();
        virtual ~Collider();

        // Get shape of the collider
        const IShape &GetShape() const { return *shape_; }

        // Get ant set this collider is trigger
        bool IsTrigger() const { return isTrigger_; }
        void SetTrigger(bool isTrigger) { isTrigger_ = isTrigger; }

        // Get and set if collision is enabled
        bool IsCollisionEnabled() const { return collisionEnabled_; }
        void SetCollisionEnabled(bool enabled) { collisionEnabled_ = enabled; }

        // Get collision result
        CollisionResult &GetCollisionResult() { return *collisionResult_; }

        // Get and set collidable component IDs
        const std::vector<size_t> &GetCollidableComponentIDs() const { return collidableComponentIDs_; }
        void AddCollidableComponentID(size_t componentID) { collidableComponentIDs_.push_back(componentID); }
        void ClearCollidableComponentIDs() { collidableComponentIDs_.clear(); }

        // Get bounding box
        const ShapeBox &GetBoundingBox() const { return boundingBox_; }
    };

} // namespace mono_physics