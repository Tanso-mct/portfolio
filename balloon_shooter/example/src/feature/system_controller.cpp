#include "example/src/pch.h"
#include "example/include/feature/system_controller.h"

#include "example/include/mode.h"

#include "example/include/feature/component_controller.h"

#include <DirectXMath.h>
using namespace DirectX;

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_physics/plugin.h"
#pragma comment(lib, "wbp_physics.lib")

const size_t &example::ControllerSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

namespace
{
    void LockCursor(UINT clientWidth, UINT clinetHeight, const HWND &windowHandle)
    {
        while (ShowCursor(FALSE) >= 0);

        POINT centerPoint = {clientWidth / 2, clinetHeight / 2};
        ClientToScreen(windowHandle, &centerPoint);

        RECT clip = { centerPoint.x, centerPoint.y, centerPoint.x+1, centerPoint.y+1 };
        ClipCursor(&clip);
        SetCursorPos(centerPoint.x, centerPoint.y);
    }

    void UnlockCursor()
    {
        while (ShowCursor(TRUE) < 0);
        ClipCursor(nullptr);
    }

} // namespace

const size_t &example::ControllerSystem::GetID() const
{
    return ControllerSystemID();
}

void example::ControllerSystem::Initialize(wb::IAssetContainer &assetContainer)
{
    
}

void example::ControllerSystem::Update(const wb::SystemArgument &args)
{
    // Get containers to use
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();

    // Get the window facade for the current window
    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    // Get the keyboard and mouse monitors
    wb::IKeyboardMonitor *keyboardMonitor = nullptr;
    wb::IMouseMonitor *mouseMonitor = nullptr;
    for (const size_t &monitorID : window.GetMonitorIDs())
    {
        wb::IMonitor &monitor = monitorContainer.Get(monitorID);
        if (keyboardMonitor == nullptr) keyboardMonitor = wb::As<wb::IKeyboardMonitor>(&monitor);
        if (mouseMonitor == nullptr) mouseMonitor = wb::As<wb::IMouseMonitor>(&monitor);
    }

    if (window.IsFocused())
    {
        ::LockCursor(window.GetClientWidth(), window.GetClientHeight(), window.GetHandle());
        window.IsFocused() = false;
    }
    else if (window.IsUnFocused())
    {
        ::UnlockCursor();
        window.IsUnFocused() = false;
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(example::ControllerComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue; // Skip if entity is null

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Entity does not have IdentityComponent.",
                    "ControllerComponent requires IdentityComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (!identity->IsActiveSelf())
        {
            // Skip if the entity is not active
            continue;
        }

        wb::IComponent *controllerComponent = entity->GetComponent(example::ControllerComponentID(), args.componentContainer_);
        example::IControllerComponent *controller = wb::As<example::IControllerComponent>(controllerComponent);

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        if (transform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Entity does not have TransformComponent.",
                    "ControllerComponent requires TransformComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        // Get camera entity
        wb::IEntity *cameraEntity = args.entityContainer_.PtrGet(controller->GetCameraEntityID());
        wb::IComponent *cameraComponent = cameraEntity->GetComponent(wbp_render::CameraComponentID(), args.componentContainer_);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        if (camera == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Camera entity does not have CameraComponent.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *cameraTransformComponent = cameraEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *cameraTransform = wb::As<wbp_transform::ITransformComponent>(cameraTransformComponent);
        if (cameraTransform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Camera entity does not have TransformComponent.",
                    "CameraComponent requires TransformComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        // Get body entity
        wb::IEntity *bodyEntity = args.entityContainer_.PtrGet(controller->GetBodyEntityID());
        wb::IComponent *bodyTransformComponent = bodyEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *bodyTransform = wb::As<wbp_transform::ITransformComponent>(bodyTransformComponent);
        if (bodyTransform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Body entity does not have TransformComponent.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

#if defined(EXAMPLE_MODE_COLLISION)

        wb::IComponent *collisionResultComponent = entity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *collisionResult = wb::As<wbp_collision::ICollisionResultComponent>(collisionResultComponent);

        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::C))
        {
            if (collisionResult != nullptr && collisionResult->IsCollided())
            {
                std::string msg = wb::CreateMessage
                ({
                    "Collided face normal : " + 
                    std::to_string(collisionResult->GetCollidedFaceNormal(0).x) + ", " +
                    std::to_string(collisionResult->GetCollidedFaceNormal(0).y) + ", " +
                    std::to_string(collisionResult->GetCollidedFaceNormal(0).z)
                });
                wb::ConsoleLog(msg);
            }
        }

        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::R))
        {
            if (keyboardMonitor->GetKeyDown(wb::KeyCode::R))
            {
                wb::IComponent *rayColliderComponent = entity->GetComponent(wbp_collision::RayColliderComponentID(), args.componentContainer_);
                wbp_collision::IRayColliderComponent *rayCollider = wb::As<wbp_collision::IRayColliderComponent>(rayColliderComponent);
                if (rayCollider == nullptr)
                {
                    std::string err = wb::CreateErrorMessage
                    (
                        __FILE__, __LINE__, __FUNCTION__,
                        {
                            "RayColliderComponent requires RayColliderComponent to be set.",
                        }
                    );
                    wb::ConsoleLogErr(err);
                    wb::ErrorNotify("WBP_CONTROLLER", err);
                    wb::ThrowRuntimeError(err);
                }

                rayCollider->SetCast(true);

                wbp_primitive::PrimitiveRay &ray = rayCollider->GetRay();
                ray.SetOrigin(cameraTransform->GetPosition());

                XMVECTOR cameraForward = cameraTransform->GetForward();
                XMFLOAT3 cameraForwardFloat3;
                XMStoreFloat3(&cameraForwardFloat3, cameraForward);
                ray.SetDirection(cameraForwardFloat3);

                ray.SetLength(100000.0f);
            }

            if (collisionResult != nullptr && collisionResult->IsCollided())
            {
                std::string msg = wb::CreateMessage
                ({
                    "Ray collided with " + std::to_string(collisionResult->GetCollidedCount()) + " entities.",
                    "Min distance: " + std::to_string(collisionResult->GetMinDistance()),
                    "Max distance: " + std::to_string(collisionResult->GetMaxDistance()),
                });
                wb::ConsoleLog(msg);
            }
        }

#endif // defined(EXAMPLE_MODE_COLLISION)

        XMFLOAT3 axis(0.0f, 0.0f, 0.0f);
        float moveValue = controller->GetSpeed() * args.deltaTime_;
        if (keyboardMonitor->GetKey(wb::KeyCode::W)) axis.z += 1;
        if (keyboardMonitor->GetKey(wb::KeyCode::S)) axis.z -= 1;
        if (keyboardMonitor->GetKey(wb::KeyCode::D)) axis.x += 1;
        if (keyboardMonitor->GetKey(wb::KeyCode::A)) axis.x -= 1;
        if (keyboardMonitor->GetKey(wb::KeyCode::Space)) axis.y += 1;
        if (keyboardMonitor->GetKey(wb::KeyCode::LShift)) axis.y -= 1;

        if (axis.x != 0.0f || axis.z != 0.0f)
        {
            // Normalize the axis
            float length = sqrtf(axis.x * axis.x + axis.y * axis.y + axis.z * axis.z);
            axis.x /= length;
            axis.y /= length;
            axis.z /= length;

            // Scale the axis by moveValue
            axis.x *= moveValue;
            axis.y *= moveValue;
            axis.z *= moveValue;
        }

        // Update body yaw
        controller->CurrentBodyYaw() = controller->CurrentCameraYaw();

        // Rotate the amount of movement by axis to match Body's Yaw
        XMVECTOR cameraHorizQuatRotVec = XMQuaternionRotationRollPitchYaw
        (
            0.0f, XMConvertToRadians(controller->CurrentBodyYaw()), 0.0f
        );
        XMVECTOR axisVec = XMLoadFloat3(&axis);
        
        axisVec = XMVector3Rotate(axisVec, cameraHorizQuatRotVec);
        XMFLOAT3 convertedAxis;
        XMStoreFloat3(&convertedAxis, axisVec);

#if defined(EXAMPLE_MODE_PHYSICS)

        // Get the RigidBodyComponent
        wb::IComponent *rigidBodyComponent = entity->GetComponent(wbp_physics::RigidBodyComponentID(), args.componentContainer_);
        wbp_physics::IRigidBodyComponent *rigidBody = wb::As<wbp_physics::IRigidBodyComponent>(rigidBodyComponent);
        if (rigidBody == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Entity does not have RigidBodyComponent.",
                    "ControllerComponent requires RigidBodyComponent to be set.",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        rigidBody->SetVelocity(convertedAxis);

#else

        if (convertedAxis.x != 0.0f || convertedAxis.y != 0.0f || convertedAxis.z != 0.0f)
        {
            transform->Translate(convertedAxis);
        }

#endif



        // Rotate camera using mouse movement
        if (mouseMonitor->GetDeltaPositionX() != 0 || mouseMonitor->GetDeltaPositionY() != 0)
        {
            controller->CurrentCameraYaw() += mouseMonitor->GetDeltaPositionX() * controller->GetSensitivity();
            controller->CurrentCameraPitch() += mouseMonitor->GetDeltaPositionY() * controller->GetSensitivity();

            cameraTransform->SetLocalRotation
            (
                DirectX::XMFLOAT3(controller->CurrentCameraPitch(), controller->CurrentCameraYaw(), 0.0f)
            );
        }

        if (convertedAxis.x != 0.0f || convertedAxis.z != 0.0f)
        {
            // Point BodyEntity in the current direction of travel
            XMVECTOR forwardVec = XMVector3Normalize(XMLoadFloat3(&convertedAxis));
            XMFLOAT3 forward;
            XMStoreFloat3(&forward, forwardVec);

            bodyTransform->SetLocalRotation
            (
                DirectX::XMFLOAT3(0.0f, XMConvertToDegrees(atan2f(forward.x, forward.z)), 0.0f)
            );
        }

    }
}

namespace example
{
    WB_REGISTER_SYSTEM(ControllerSystem, ControllerSystemID())

} // namespace example