#pragma once
#include "wbp_physics/include/dll_config.h"
#include "windows_base/windows_base.h"

namespace wbp_physics
{
    const WBP_PHYSICS_API size_t &RigidBodySystemID();

    class WBP_PHYSICS_API RigidBodySystem : public wb::ISystem
    {
    public:
        RigidBodySystem() = default;
        ~RigidBodySystem() override = default;

        RigidBodySystem(const RigidBodySystem &) = delete;
        RigidBodySystem &operator=(const RigidBodySystem &) = delete;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;
    };

    const WBP_PHYSICS_API size_t &RigidBodyResponseSystemID();

    class WBP_PHYSICS_API RigidBodyResponseSystem : public wb::ISystem
    {
    public:
        RigidBodyResponseSystem() = default;
        ~RigidBodyResponseSystem() override = default;

        RigidBodyResponseSystem(const RigidBodyResponseSystem &) = delete;
        RigidBodyResponseSystem &operator=(const RigidBodyResponseSystem &) = delete;

        /***************************************************************************************************************
         * ISystem implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;
        void Initialize(wb::IAssetContainer &assetContainer) override;
        void Update(const wb::SystemArgument &args) override;
    };

} // namespace wbp_physics