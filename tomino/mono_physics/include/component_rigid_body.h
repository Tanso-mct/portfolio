#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>
#include <memory>

namespace mono_physics
{
    constexpr size_t ComponentRigidBodyMaxCount = 1000;
    class MONO_PHYSICS_API ComponentRigidBody
    {
    private:
        // Attached collider
        bool colliderAttached_ = false;
        size_t attachedColliderComponentID_ = 0;

        // Rigid body properties
        bool isStatic_ = false;
        float mass_ = 1.0f;
        float staticFriction_ = 0.5f;
        float dynamicFriction_ = 0.5f;
        DirectX::XMFLOAT3 velocity_ = {0.0f, 0.0f, 0.0f};

    public:
        ComponentRigidBody();
        ~ComponentRigidBody();

        struct SetupParam
        {
            bool isStatic = false;
            float mass = 1.0f;
            float staticFriction = 0.5f;
            float dynamicFriction = 0.5f;
        };
        void Setup(SetupParam &param);

        // Get and set attached collider component ID
        void SetAttachedColliderComponentID(size_t colliderComponentID);
        void RemoveAttachedColliderComponentID();
        bool GetAttachedColliderComponentID(size_t &colliderComponentID) const;

        // Get and set rigid body properties
        bool IsStatic() const { return isStatic_; }
        void SetStatic(bool isStatic) { isStatic_ = isStatic; }

        float GetMass() const { return mass_; }
        void SetMass(float mass) { mass_ = mass; }

        float GetStaticFriction() const { return staticFriction_; }
        void SetStaticFriction(float staticFriction) { staticFriction_ = staticFriction; }

        float GetDynamicFriction() const { return dynamicFriction_; }
        void SetDynamicFriction(float dynamicFriction) { dynamicFriction_ = dynamicFriction; }

        const DirectX::XMFLOAT3& GetVelocity() const { return velocity_; }
        void SetVelocity(const DirectX::XMFLOAT3 &velocity) { velocity_ = velocity; }

    };

    extern MONO_PHYSICS_API riaecs::ComponentRegistrar
    <ComponentRigidBody, ComponentRigidBodyMaxCount> ComponentRigidBodyID;

} // namespace mono_physics