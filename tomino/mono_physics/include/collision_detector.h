#pragma once

#include <utility>
#include <memory>
#include <unordered_map>

#include "mono_transform/mono_transform.h"

#include "mono_physics/include/dll_config.h"
#include "mono_physics/include/collider.h"
#include "mono_physics/include/component_rigid_body.h"

namespace mono_physics
{
    class ColliderPair
    {
    public:
        ColliderPair(size_t colliderAID, size_t colliderBID) :
            colliderAID_(colliderAID), colliderBID_(colliderBID) 
        {
        }

        ~ColliderPair() = default;

        size_t GetColliderAID() const { return colliderAID_; }
        size_t GetColliderBID() const { return colliderBID_; }

        bool operator==(const ColliderPair &other) const
        {
            return 
                (colliderAID_ == other.colliderAID_ && colliderBID_ == other.colliderBID_) ||
                (colliderAID_ == other.colliderBID_ && colliderBID_ == other.colliderAID_);
        }        

        bool operator!=(const ColliderPair &other) const
        {
            return !(*this == other);
        }

    private:
        size_t colliderAID_;
        size_t colliderBID_;
    };

}

namespace std
{
    template <>
    struct hash<mono_physics::ColliderPair>
    {
        std::size_t operator()(const mono_physics::ColliderPair &pair) const noexcept
        {
            return std::hash<size_t>()(pair.GetColliderAID()) ^ (std::hash<size_t>()(pair.GetColliderBID()) << 1);
        }
    };
}

namespace mono_physics
{
    class CollisionDetector
    {
    public:
        CollisionDetector() = default;
        virtual ~CollisionDetector() = default;

        // Detect collisions between two colliders given their world matrices
        // Return true if a collision is detected, false otherwise
        virtual bool DetectCollisions(
            const riaecs::Entity& entityA, Collider& colliderA, mono_transform::ComponentTransform& transformA,
            const riaecs::Entity& entityB, Collider& colliderB, mono_transform::ComponentTransform& transformB) = 0;
    };
    
    class MONO_PHYSICS_API CollisionDetectorRegistry
    {
    public:
        CollisionDetectorRegistry() = default;
        ~CollisionDetectorRegistry() = default;

        // Delete copy constructor and assignment operator
        CollisionDetectorRegistry(const CollisionDetectorRegistry&) = delete;
        CollisionDetectorRegistry& operator=(const CollisionDetectorRegistry&) = delete;

        void Register(ColliderPair pair, std::unique_ptr<CollisionDetector> detector);
        CollisionDetector& Get(const ColliderPair &pair) const;

    private:
        std::unordered_map<ColliderPair, std::unique_ptr<CollisionDetector>> detectors_;
    };

} // namespace mono_physics