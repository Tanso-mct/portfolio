#pragma once
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/collider.h"

#include "wbp_primitive/include/primitive.h"

namespace wbp_collision
{
    class IRayColliderComponent : public wb::IComponent, public ICollider
    {
    public:
        virtual ~IRayColliderComponent() = default;

        virtual wbp_primitive::PrimitiveRay &GetRay() = 0;

        virtual bool IsCast() const = 0;
        virtual void SetCast(bool casted) = 0;

        virtual bool IsCasting() const = 0;
        virtual void SetCasting(bool casting) = 0;

        virtual void AddTargetComponentID(const size_t &id) = 0;
        virtual void RemoveTargetComponentID(const size_t &id) = 0;
        virtual void ClearTargetComponentIDs() = 0;
        virtual const std::vector<size_t> &GetTargetComponentIDs() const = 0;
    }; 

} // namespace wbp_collision