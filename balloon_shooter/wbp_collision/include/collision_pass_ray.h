#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/colllision_pass.h"

namespace wbp_collision
{
    class WBP_COLLISION_API CollisionPassRay : public ICollisionPass
    {
    public:
        CollisionPassRay() = default;
        ~CollisionPassRay() override = default;

        CollisionPassRay(const CollisionPassRay &) = delete;
        CollisionPassRay &operator=(const CollisionPassRay &) = delete;

        void Execute(const wb::SystemArgument &args) override;
    };

} // namespace wbp_collision