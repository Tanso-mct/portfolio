#pragma once
#include "mono_object_controller/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <DirectXMath.h>

#include "mono_input_monitor/include/keyboard_monitor.h"

namespace mono_object_controller
{
    constexpr mono_input_monitor::KeyCode KEYCODE_FORWARD = mono_input_monitor::KeyCode::W;
    constexpr mono_input_monitor::KeyCode KEYCODE_BACK = mono_input_monitor::KeyCode::S;
    constexpr mono_input_monitor::KeyCode KEYCODE_LEFT = mono_input_monitor::KeyCode::A;
    constexpr mono_input_monitor::KeyCode KEYCODE_RIGHT = mono_input_monitor::KeyCode::D;

    constexpr float DEFAULT_MOVE_SPEED = 2.0f;

    constexpr size_t ComponentObjectControllerMaxCount = 100;
    class MONO_OBJECT_CONTROLLER_API ComponentObjectController
    {
    private:
        riaecs::Entity handlerWindowEntity_ = riaecs::Entity();

        mono_input_monitor::KeyCode forwardKey_ = KEYCODE_FORWARD;
        mono_input_monitor::KeyCode backKey_ = KEYCODE_BACK;
        mono_input_monitor::KeyCode leftKey_ = KEYCODE_LEFT;
        mono_input_monitor::KeyCode rightKey_ = KEYCODE_RIGHT;

        float moveSpeed_ = DEFAULT_MOVE_SPEED;

        // Normalized direction vector
        DirectX::XMFLOAT3 moveDirection_ = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

    public:
        ComponentObjectController();
        ~ComponentObjectController();

        struct SetupParam
        {
            riaecs::Entity handlerWindowEntity = riaecs::Entity();
            mono_input_monitor::KeyCode forwardKey = KEYCODE_FORWARD;
            mono_input_monitor::KeyCode backKey = KEYCODE_BACK;
            mono_input_monitor::KeyCode leftKey = KEYCODE_LEFT;
            mono_input_monitor::KeyCode rightKey = KEYCODE_RIGHT;

            float moveSpeed = DEFAULT_MOVE_SPEED;
            float moveDirectionYaw = 0.0f; // Yaw angle in degrees
        };
        void Setup(SetupParam &param);

        const riaecs::Entity &GetHandlerWindowEntity() const { return handlerWindowEntity_; }
        void SetHandlerWindowEntity(const riaecs::Entity &entity) { handlerWindowEntity_ = entity; }

        mono_input_monitor::KeyCode GetForwardKeyCode() const { return forwardKey_; }
        void SetForwardKeyCode(mono_input_monitor::KeyCode keyCode) { forwardKey_ = keyCode; }

        mono_input_monitor::KeyCode GetBackKeyCode() const { return backKey_; }
        void SetBackKeyCode(mono_input_monitor::KeyCode keyCode) { backKey_ = keyCode; }

        mono_input_monitor::KeyCode GetLeftKeyCode() const { return leftKey_; }
        void SetLeftKeyCode(mono_input_monitor::KeyCode keyCode) { leftKey_ = keyCode; }

        mono_input_monitor::KeyCode GetRightKeyCode() const { return rightKey_; }
        void SetRightKeyCode(mono_input_monitor::KeyCode keyCode) { rightKey_ = keyCode; }

        float GetMoveSpeed() const { return moveSpeed_; }
        void SetMoveSpeed(float speed) { moveSpeed_ = speed; }

        const DirectX::XMFLOAT3 &GetMoveDirection() const { return moveDirection_; }
        void SetMoveDirection(const DirectX::XMFLOAT3 &direction) { moveDirection_ = direction; }
    };

    extern MONO_OBJECT_CONTROLLER_API riaecs::ComponentRegistrar
    <ComponentObjectController, ComponentObjectControllerMaxCount> ComponentObjectControllerID;

} // namespace mono_object_controller