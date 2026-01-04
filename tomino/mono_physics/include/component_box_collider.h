#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_physics/include/collider.h"
#include "mono_physics/include/shape.h"

#include <DirectXMath.h>

namespace mono_physics
{
    class MONO_PHYSICS_API BoxCollisionResult : public CollisionResult
    {
    private:
        std::vector<DirectX::XMFLOAT3> collisionNormals_;
        
    public:
        BoxCollisionResult();
        ~BoxCollisionResult() override;

        virtual void Clear() override;

        const std::vector<DirectX::XMFLOAT3> &GetCollisionNormals() const { return collisionNormals_; }
        void AddCollisionNormal(const DirectX::XMFLOAT3 &normal) { collisionNormals_.push_back(normal); }
        void ClearCollisionNormals() { collisionNormals_.clear(); }
    };

    constexpr size_t ComponentBoxColliderMaxCount = 5000;
    class MONO_PHYSICS_API ComponentBoxCollider : public Collider
    {
    public:
        ComponentBoxCollider();
        ~ComponentBoxCollider() override;

        struct SetupParam
        {
            bool isTrigger = false;
            std::unique_ptr<CollisionResult> collisionResult = std::make_unique<BoxCollisionResult>();
            std::unique_ptr<ShapeBox> box = std::make_unique<ShapeBox>();
        };
        void Setup(SetupParam &param);

        const ShapeBox &GetBox() const;
        void SetBox(std::unique_ptr<ShapeBox> box);

        const BoxCollisionResult &GetBoxCollisionResult() const;
        void SetBoxCollisionResult(std::unique_ptr<BoxCollisionResult> result);
    };
    extern MONO_PHYSICS_API riaecs::ComponentRegistrar
    <ComponentBoxCollider, ComponentBoxColliderMaxCount> ComponentBoxColliderID;

} // namespace mono_physics