#pragma once
#include "wbp_collision/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_collision/include/interfaces/colllision_pass.h"

namespace wbp_collision
{
    const WBP_COLLISION_API size_t &CollisionSystemID();

    class WBP_COLLISION_API CollisionSystem : public wb::ISystem
    {
    private:
        std::vector<std::unique_ptr<ICollisionPass>> passes_;

    public:
        CollisionSystem();
        ~CollisionSystem() override = default;

        CollisionSystem(const CollisionSystem &) = delete;
        CollisionSystem &operator=(const CollisionSystem &) = delete;

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;
    };

} // namespace wbp_collision