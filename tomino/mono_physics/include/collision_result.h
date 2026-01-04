#pragma once
#include "mono_physics/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace mono_physics
{
    class MONO_PHYSICS_API CollisionResult
    {
    protected:
        bool collided_ = false;
        std::vector<riaecs::Entity> collidedEntities_;
        
    public:
        CollisionResult();
        virtual ~CollisionResult();

        bool IsCollided() const { return collided_; }
        void SetCollided(bool collided) { collided_ = collided; }

        const std::vector<riaecs::Entity> &GetCollidedEntities() const { return collidedEntities_; }
        void AddCollidedEntity(riaecs::Entity entity) { collidedEntities_.push_back(entity); }
        void ClearCollidedEntities() { collidedEntities_.clear(); }

        virtual void Clear();
    };

} // namespace mono_physics