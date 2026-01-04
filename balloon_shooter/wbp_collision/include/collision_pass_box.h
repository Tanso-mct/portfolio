#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/colllision_pass.h"

namespace wbp_collision
{
    class WBP_COLLISION_API CollisionPassBox : public ICollisionPass
    {
    public:
        CollisionPassBox() = default;
        ~CollisionPassBox() override = default;

        CollisionPassBox(const CollisionPassBox &) = delete;
        CollisionPassBox &operator=(const CollisionPassBox &) = delete;

        void Execute(const wb::SystemArgument &args) override;
    };

} // namespace wbp_collision