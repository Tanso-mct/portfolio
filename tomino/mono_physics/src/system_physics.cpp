#include "mono_physics/src/pch.h"
#include "mono_physics/include/system_physics.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "mono_transform.lib")

using namespace DirectX;

#include "mono_physics/include/shape.h"
#include "mono_physics/include/collider.h"
#include "mono_physics/include/component_rigid_body.h"
#include "mono_physics/include/component_box_collider.h"

#include "mono_physics/include/detector_box_vs_box.h"
#include "mono_physics/include/resolver_box.h"

namespace system_physics
{

class EntityPair
{
public:
    EntityPair(const riaecs::Entity &entityA, const riaecs::Entity &entityB)
        : entityA_(entityA), entityB_(entityB) {}

    const riaecs::Entity &GetEntityA() const { return entityA_; }
    const riaecs::Entity &GetEntityB() const { return entityB_; }

    bool operator==(const EntityPair &other) const
    {
        auto a1 = entityA_.GetIndex();
        auto b1 = entityB_.GetIndex();
        auto a2 = other.entityA_.GetIndex();
        auto b2 = other.entityB_.GetIndex();

        return (a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2);
    }

private:
    riaecs::Entity entityA_;
    riaecs::Entity entityB_;
};

} // namespace system_physics

namespace std
{
    template <>
    struct hash<system_physics::EntityPair>
    {
        std::size_t operator()(const system_physics::EntityPair &pair) const noexcept
        {
            auto makeKey = [](const riaecs::Entity& e) {
                size_t i = e.GetIndex();
                size_t g = e.GetGeneration();
                std::size_t h1 = std::hash<size_t>()(i);
                std::size_t h2 = std::hash<size_t>()(g);
                return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
            };

            auto keyA = makeKey(pair.GetEntityA());
            auto keyB = makeKey(pair.GetEntityB());
            if (keyA > keyB) std::swap(keyA, keyB);

            std::size_t h1 = keyA;
            std::size_t h2 = keyB;
            return h1 ^ (h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2));
        }
    };
}

mono_physics::SystemPhysics::SystemPhysics() :
    spatialGrid_(girdCellSize_)
{
    // Register collision detectors  

    { // Box vs Box
        ColliderPair pair = {mono_physics::ComponentBoxColliderID(), mono_physics::ComponentBoxColliderID()};
        std::unique_ptr<mono_physics::CollisionDetector> detector = std::make_unique<mono_physics::DetectorBoxVsBox>();
        collisionDetectorRegistry_.Register(pair, std::move(detector));
    }

    // Register collision resolvers
    
    { // Box
        size_t colliderComponentID = mono_physics::ComponentBoxColliderID();
        std::unique_ptr<mono_physics::CollisionResolver> resolver = std::make_unique<mono_physics::ResolverBox>();
        CollisionResolverRegistry_.Register(colliderComponentID, std::move(resolver));
    }
}

mono_physics::SystemPhysics::~SystemPhysics()
{
}

bool mono_physics::SystemPhysics::Update
(
    riaecs::IECSWorld &ecsWorld, 
    riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    // Delta time calculation
    deltaTimeProvider_.UpdateTime();
    float deltaTime = deltaTimeProvider_.GetDeltaTime();

    // Clear spatial grid
    spatialGrid_.Clear();

    // Register all colliders to spatial grid
    // And also store velocity in rigid body, clear previous collision results
    for (const riaecs::Entity &entity : ecsWorld.View(mono_physics::ComponentRigidBodyID())())
    {
        // Get the identity component
        mono_identity::ComponentIdentity *identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);

        if (!identity->IsActiveSelf()) // Skip if not active
            continue;

        mono_physics::ComponentRigidBody *rigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Check if the entity has a collider attached
        size_t colliderComponentID = 0;
        bool hasCollider = rigidBody->GetAttachedColliderComponentID(colliderComponentID);
        assert(hasCollider); // Rigid body must have a collider attached

        // If it has a collider, get the collider component
        mono_physics::Collider *collider 
            = riaecs::GetComponentWithCheck<mono_physics::Collider>(
                ecsWorld, entity, colliderComponentID, "Collider", RIAECS_LOG_LOC);

        // Get the transform component
        mono_transform::ComponentTransform *transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        // Get bounding box
        const mono_physics::ShapeBox &boundingBox = collider->GetBoundingBox();

        // Register to spatial grid
        spatialGrid_.RegisterAABB(entity, boundingBox, transform->GetWorldMatrixNoRot());

        // Store velocity in rigid body for later use
        XMFLOAT3 velocity = XMFLOAT3(
            transform->GetPos().x - transform->GetLastPos().x,
            transform->GetPos().y - transform->GetLastPos().y,
            transform->GetPos().z - transform->GetLastPos().z);
        rigidBody->SetVelocity(velocity);

        // Clear previous collision results
        collider->GetCollisionResult().Clear();
    }

    // Query potential collisions
    std::unordered_set<system_physics::EntityPair> potentialCollisionPairs;
    for (const riaecs::Entity &entity : ecsWorld.View(mono_physics::ComponentRigidBodyID())())
    {
        // Get the identity component
        mono_identity::ComponentIdentity *identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);
            
        if (!identity->IsActiveSelf()) // Skip if not active
            continue;

        mono_physics::ComponentRigidBody *rigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Check if the entity has a collider attached
        size_t colliderComponentID = 0;
        bool hasCollider = rigidBody->GetAttachedColliderComponentID(colliderComponentID);
        assert(hasCollider); // Rigid body must have a collider attached

        // If it has a collider, get the collider component
        mono_physics::Collider *collider 
            = riaecs::GetComponentWithCheck<mono_physics::Collider>(
                ecsWorld, entity, colliderComponentID, "Collider", RIAECS_LOG_LOC);

        // Get the transform component
        mono_transform::ComponentTransform *transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        // Get bounding box
        const mono_physics::ShapeBox &boundingBox = collider->GetBoundingBox();

        // Query potential collisions
        std::vector<riaecs::Entity> potentialCollisions 
            = spatialGrid_.QueryNearby(entity, boundingBox, transform->GetWorldMatrixNoRot());

        // Get collidable component IDs
        const std::vector<size_t> &collidableComponentIDs = collider->GetCollidableComponentIDs();

        for (const riaecs::Entity &other : potentialCollisions)
        {
            assert(other != entity); // Should not be the same entity

            // Check if the other entity has a collider attached
            mono_physics::ComponentRigidBody *otherRigidBody
            = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
                ecsWorld, other, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

            size_t otherColliderComponentID = 0;
            bool otherHasCollider = otherRigidBody->GetAttachedColliderComponentID(otherColliderComponentID);
            assert(otherHasCollider); // Other rigid body must have a collider attached

            // Check if the other entity has any of the collidable component IDs
            for (size_t id : collidableComponentIDs)
            {
                if (ecsWorld.HasComponent(other, id))
                {
                    // Create entity pair
                    system_physics::EntityPair pair(entity, other);

                    // Add to potential collision pairs
                    potentialCollisionPairs.insert(pair);

                    break; // No need to check further
                }
            }
        }
    }

    // Narrowphase collision detection
    std::unordered_set<riaecs::Entity> collidedEntities;
    for (const system_physics::EntityPair& pair : potentialCollisionPairs)
    {
        mono_physics::ComponentRigidBody *rigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, pair.GetEntityA(), mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Get the collider component
        size_t colliderComponentID = 0;
        bool hasCollider = rigidBody->GetAttachedColliderComponentID(colliderComponentID);
        assert(hasCollider); // Should have a collider

        mono_physics::ComponentRigidBody *otherRigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, pair.GetEntityB(), mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Get the other collider component
        size_t otherColliderComponentID = 0;
        bool otherHasCollider = otherRigidBody->GetAttachedColliderComponentID(otherColliderComponentID);
        assert(otherHasCollider); // Should have a collider

        // Get the collider components
        mono_physics::Collider *colliderA = riaecs::GetComponentWithCheck<mono_physics::Collider>(
            ecsWorld, pair.GetEntityA(), colliderComponentID, "Collider", RIAECS_LOG_LOC);
        mono_physics::Collider *colliderB = riaecs::GetComponentWithCheck<mono_physics::Collider>(
            ecsWorld, pair.GetEntityB(), otherColliderComponentID, "Collider", RIAECS_LOG_LOC);

        // Get the transform components
        mono_transform::ComponentTransform *transformA
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, pair.GetEntityA(), mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);
        mono_transform::ComponentTransform *transformB
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, pair.GetEntityB(), mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        // Create collider pair
        mono_physics::ColliderPair colliderPair(colliderComponentID, otherColliderComponentID);

        // Get the collision detector
        mono_physics::CollisionDetector& detector = collisionDetectorRegistry_.Get(colliderPair);

        bool playerDetected = false;

        // Get identity component
        mono_identity::ComponentIdentity *identityA
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, pair.GetEntityA(), mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);

        if (identityA->GetName() == "Player")
            playerDetected = true;

        mono_identity::ComponentIdentity *identityB
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, pair.GetEntityB(), mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);

        if (identityB->GetName() == "Player")
            playerDetected = true;

        // Detect collision
        bool isColliding = detector.DetectCollisions(
            pair.GetEntityA(), *colliderA, *transformA,
            pair.GetEntityB(), *colliderB, *transformB);

        if (isColliding && playerDetected)
        {
            int a = 0;
        }

        if (isColliding) // If colliding, add to the collided entities
        {
            collidedEntities.insert(pair.GetEntityA());
            collidedEntities.insert(pair.GetEntityB());
        }
    }

    // Collision resolution
    for (const riaecs::Entity& entity : collidedEntities)
    {
        mono_physics::ComponentRigidBody *rigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Get the collider component
        size_t colliderComponentID = 0;
        bool hasCollider = rigidBody->GetAttachedColliderComponentID(colliderComponentID);
        assert(hasCollider); // Should have a collider
        Collider *collider = riaecs::GetComponentWithCheck<mono_physics::Collider>(
            ecsWorld, entity, colliderComponentID, "Collider", RIAECS_LOG_LOC);

        // Get the resolver for this collider
        mono_physics::CollisionResolver &resolver = CollisionResolverRegistry_.Get(colliderComponentID);

        // Get the collision result
        mono_physics::CollisionResult &collisionResult = collider->GetCollisionResult();
        for (size_t i = 0; i < collisionResult.GetCollidedEntities().size(); i++)
        {
            // Get the other entity
            riaecs::Entity otherEntity = collisionResult.GetCollidedEntities()[i];

            // Get the other rigid body
            mono_physics::ComponentRigidBody *otherRigidBody
            = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
                ecsWorld, otherEntity, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

            // Get the other collider
            size_t otherColliderComponentID = 0;
            bool otherHasCollider = otherRigidBody->GetAttachedColliderComponentID(otherColliderComponentID);
            assert(otherHasCollider); // Should have a collider

            Collider *otherCollider = riaecs::GetComponentWithCheck<mono_physics::Collider>(
                ecsWorld, otherEntity, otherColliderComponentID, "Collider", RIAECS_LOG_LOC);

            // Resolve collision
            resolver.ResolveCollision(*collider, *rigidBody, i, *otherCollider, *otherRigidBody);
        }
    }

    // Update transform
    for (const riaecs::Entity &entity : ecsWorld.View(mono_physics::ComponentRigidBodyID())())
    {
        // Get the identity component
        mono_identity::ComponentIdentity *identity
        = riaecs::GetComponentWithCheck<mono_identity::ComponentIdentity>(
            ecsWorld, entity, mono_identity::ComponentIdentityID(), "ComponentIdentity", RIAECS_LOG_LOC);

        if (!identity->IsActiveSelf()) // Skip if not active
            continue;

        mono_physics::ComponentRigidBody *rigidBody
        = riaecs::GetComponentWithCheck<mono_physics::ComponentRigidBody>(
            ecsWorld, entity, mono_physics::ComponentRigidBodyID(), "ComponentRigidBody", RIAECS_LOG_LOC);

        // Get the transform component
        mono_transform::ComponentTransform *transform
        = riaecs::GetComponentWithCheck<mono_transform::ComponentTransform>(
            ecsWorld, entity, mono_transform::ComponentTransformID(), "ComponentTransform", RIAECS_LOG_LOC);

        if (rigidBody->IsStatic()) // If static, do not update
            continue;

        // Update position based on velocity
        XMFLOAT3 velocity = rigidBody->GetVelocity();
        XMFLOAT3 newPos = XMFLOAT3(
            transform->GetLastPos().x + velocity.x,
            transform->GetLastPos().y + velocity.y,
            transform->GetLastPos().z + velocity.z);
        transform->SetPos(newPos, ecsWorld);
    }

    return true; // Continue running
}

MONO_PHYSICS_API riaecs::SystemFactoryRegistrar
<mono_physics::SystemPhysics> mono_physics::SystemPhysicsID;