#pragma once
#include "wbp_physics/include/dll_config.h"
#include "windows_base/windows_base.h"

#include "wbp_physics/include/interfaces/component_rigid_body.h"

namespace wbp_physics
{
    const WBP_PHYSICS_API size_t &RigidBodyComponentID();

    class WBP_PHYSICS_API RigidBodyComponent : public IRigidBodyComponent
    {
    private:
        bool affectedByOther_ = true;
        bool kinematic_ = false;
        DirectX::XMFLOAT3 initialPos_ = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 terminalPos_ = { 0.0f, 0.0f, 0.0f };

        DirectX::XMFLOAT3 velocity_ = { 0.0f, 0.0f, 0.0f };

    public:
        RigidBodyComponent() = default;
        ~RigidBodyComponent() override = default;

        /***************************************************************************************************************
         * IComponent implementation
        /**************************************************************************************************************/

        const size_t &GetID() const override;

        /***************************************************************************************************************
         * IRigidBodyComponent implementation
        /**************************************************************************************************************/

        bool IsAffectedByOther() const override { return affectedByOther_; }
        void SetAffectedByOther(bool affected) override { affectedByOther_ = affected; }

        bool IsKinematic() const override { return kinematic_; }
        void SetKinematic(bool kinematic) override { kinematic_ = kinematic; }

        const DirectX::XMFLOAT3 &GetInitialPos() override { return initialPos_; }
        void SetInitialPos(const DirectX::XMFLOAT3 &pos) override { initialPos_ = pos; }

        const DirectX::XMFLOAT3 &GetTerminalPos() override { return terminalPos_; }
        void SetTerminalPos(const DirectX::XMFLOAT3 &pos) override { terminalPos_ = pos; }

        const DirectX::XMFLOAT3 &GetVelocity() override { return velocity_; }
        void SetVelocity(const DirectX::XMFLOAT3 &velocity) override { velocity_ = velocity; }
    };

} // namespace wbp_physics