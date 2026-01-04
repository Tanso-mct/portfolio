#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/system_player.h"

#include "balloon_shooter/include/feature/component_player.h"
#include "balloon_shooter/include/feature/component_balloon.h"
#include "balloon_shooter/include/feature/component_map.h"
#include "balloon_shooter/include/feature/component_ui_player.h"

#include "balloon_shooter/include/feature/shared_game_state.h"
#include "balloon_shooter/include/feature/game_state_clear.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_physics/plugin.h"
#pragma comment(lib, "wbp_physics.lib")

#include "wbp_locator/plugin.h"
#pragma comment(lib, "wbp_locator.lib")

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

const size_t &balloon_shooter::PlayerSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &balloon_shooter::PlayerSystem::GetID() const
{
    return PlayerSystemID();
}

void balloon_shooter::PlayerSystem::Initialize(wb::IAssetContainer &assetContainer)
{
}

void balloon_shooter::PlayerSystem::Update(const wb::SystemArgument &args)
{
    // 使用するコンテナを取得
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();
    wb::IAssetContainer &assetContainer = args.containerStorage_.GetContainer<wb::IAssetContainer>();
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // ウィンドウのファサードを取得
    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    // キーボードとマウスのモニターを取得
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

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        if (identity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "PlayerComponentを追加する場合、IdentityComponentも追加する必要があります。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (!identity->IsActiveSelf())
        {
            // Entityが非アクティブな場合はスキップ
            continue;
        }

        wb::IComponent *playerComponent = entity->GetComponent(balloon_shooter::PlayerComponentID(), args.componentContainer_);
        balloon_shooter::IPlayerComponent *player = wb::As<balloon_shooter::IPlayerComponent>(playerComponent);

        // Camera
        wb::IEntity *cameraEntity = args.entityContainer_.PtrGet(player->GetCameraEntityID());

        wb::IComponent *cameraComponent = cameraEntity->GetComponent(wbp_render::CameraComponentID(), args.componentContainer_);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        if (camera == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Camera entityにCameraComponentがありません。",
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
                    "Camera entityにTransformComponentがありません。",
                    "CameraComponentを追加する場合、TransformComponentも追加する必要があります。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        // Body
        wb::IEntity *bodyEntity = args.entityContainer_.PtrGet(player->GetBodyEntityID());

        wb::IComponent *bodyTransformComponent = bodyEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *bodyTransform = wb::As<wbp_transform::ITransformComponent>(bodyTransformComponent);
        if (bodyTransform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Body entityにTransformComponentがありません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *bodyRigidBodyComponent = bodyEntity->GetComponent(wbp_physics::RigidBodyComponentID(), args.componentContainer_);
        wbp_physics::IRigidBodyComponent *bodyRigidBody = wb::As<wbp_physics::IRigidBodyComponent>(bodyRigidBodyComponent);
        if (bodyRigidBody == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "PlayerComponentにRigidBodyComponentがありません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *bodyCollisionResultComponent = bodyEntity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *bodyCollisionResult = wb::As<wbp_collision::ICollisionResultComponent>(bodyCollisionResultComponent);
        if (bodyCollisionResult == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "PlayerComponentにCollisionResultComponentがありません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        // Gun
        wb::IEntity *gunEntity = args.entityContainer_.PtrGet(player->GetGunEntityID());
        if (gunEntity == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "PlayerComponentにGunEntityが設定されていません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        wb::IComponent *gunTransformComponent = gunEntity->GetComponent(wbp_transform::TransformComponentID(), args.componentContainer_);
        wbp_transform::ITransformComponent *gunTransform = wb::As<wbp_transform::ITransformComponent>(gunTransformComponent);
        if (gunTransform == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "Gun entityにTransformComponentがありません。",
                    "GunComponentを追加する場合、TransformComponentも追加する必要があります。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (!isStarted)
        {
            // BodyからCameraのオフセットを取得
            XMVECTOR cameraOffsetVec 
                = XMLoadFloat3(&cameraTransform->GetPosition()) - XMLoadFloat3(&bodyTransform->GetPosition());
            XMFLOAT3 cameraOffset;
            XMStoreFloat3(&cameraOffset, cameraOffsetVec);
            player->SetCameraOffset(cameraOffset);

            isStarted = true;
        }

        for (size_t i = 0; i < bodyCollisionResult->GetCollidedCount(); i++)
        {
            wb::IEntity *collidedEntity = args.entityContainer_.PtrGet(bodyCollisionResult->GetCollidedEntityID(i));
            if (collidedEntity == nullptr) continue;

            if (collidedEntity->GetComponent(balloon_shooter::MapBottomComponentID(), args.componentContainer_) != nullptr)
            {
                wb::IComponent *cameraLocatorComponent = cameraEntity->GetComponent(wbp_locator::LocatorComponentID(), args.componentContainer_);
                wbp_locator::ILocatorComponent *cameraLocator = wb::As<wbp_locator::ILocatorComponent>(cameraLocatorComponent);
                if (cameraLocator == nullptr)
                {
                    std::string err = wb::CreateErrorMessage
                    (
                        __FILE__, __LINE__, __FUNCTION__,
                        {
                            "Camera entityにLocatorComponentがありません。",
                        }
                    );
                    wb::ConsoleLogErr(err);
                    wb::ErrorNotify("WBP_CONTROLLER", err);
                    wb::ThrowRuntimeError(err);
                }

                wb::IComponent *bodyLocatorComponent = bodyEntity->GetComponent(wbp_locator::LocatorComponentID(), args.componentContainer_);
                wbp_locator::ILocatorComponent *bodyLocator = wb::As<wbp_locator::ILocatorComponent>(bodyLocatorComponent);
                if (bodyLocator == nullptr)
                {
                    std::string err = wb::CreateErrorMessage
                    (
                        __FILE__, __LINE__, __FUNCTION__,
                        {
                            "Body entityにLocatorComponentがありません。",
                        }
                    );
                    wb::ConsoleLogErr(err);
                    wb::ErrorNotify("WBP_CONTROLLER", err);
                    wb::ThrowRuntimeError(err);
                }

                {
                    wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(cameraLocator->GetLocatorAssetID());
                    wbp_locator::ILocatorAsset *locatorAsset = wb::As<wbp_locator::ILocatorAsset>(&asset());
                    if (locatorAsset == nullptr)
                    {
                        std::string err = wb::CreateErrorMessage
                        (
                            __FILE__, __LINE__, __FUNCTION__,
                            {
                                "LocatorAssetが見つかりません。",
                            }
                        );
                        wb::ConsoleLogErr(err);
                        wb::ErrorNotify("WBP_CONTROLLER", err);
                        wb::ThrowRuntimeError(err);
                    }

                    cameraTransform->SetLocalPosition(locatorAsset->GetLocates()[cameraLocator->GetLocateTargetIndex()]);
                }

                {
                    wb::LockedRef<wb::IAsset> asset = assetContainer.ThreadSafeGet(bodyLocator->GetLocatorAssetID());
                    wbp_locator::ILocatorAsset *locatorAsset = wb::As<wbp_locator::ILocatorAsset>(&asset());
                    if (locatorAsset == nullptr)
                    {
                        std::string err = wb::CreateErrorMessage
                        (
                            __FILE__, __LINE__, __FUNCTION__,
                            {
                                "LocatorAssetが見つかりません。",
                            }
                        );
                        wb::ConsoleLogErr(err);
                        wb::ErrorNotify("WBP_CONTROLLER", err);
                        wb::ThrowRuntimeError(err);
                    }

                    bodyTransform->SetLocalPosition(locatorAsset->GetLocates()[bodyLocator->GetLocateTargetIndex()]);
                }
            }
        }

        // すでにレイがキャストされている場合
        if (player->IsRayCasted())
        {
            wb::IEntity *rayEntity = args.entityContainer_.PtrGet(player->GetRayEntityID());
            if (rayEntity == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "PlayerComponentにRayEntityが設定されていません。",
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_CONTROLLER", err);
                wb::ThrowRuntimeError(err);
            }

            wb::IComponent *rayColliderComponent = rayEntity->GetComponent(wbp_collision::RayColliderComponentID(), args.componentContainer_);
            wbp_collision::IRayColliderComponent *rayCollider = wb::As<wbp_collision::IRayColliderComponent>(rayColliderComponent);
            if (rayCollider == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "Ray EntityにRayColliderComponentがありません。",
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_CONTROLLER", err);
                wb::ThrowRuntimeError(err);
            }

            wb::IComponent *rayCollisionResultComponent = rayEntity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
            wbp_collision::ICollisionResultComponent *rayCollisionResult = wb::As<wbp_collision::ICollisionResultComponent>(rayCollisionResultComponent);
            if (rayCollisionResult == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "Ray EntityにRayCollisionResultComponentがありません。",
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_CONTROLLER", err);
                wb::ThrowRuntimeError(err);
            }

            const wb::IOptionalValue *minDistanceEntityID = rayCollisionResult->GetMinDistanceColliedEntityID();
            if (minDistanceEntityID != nullptr && minDistanceEntityID->IsValid())
            {
                wb::IEntity *minDistanceEntity = args.entityContainer_.PtrGet(*minDistanceEntityID);

                // 最も近いEntityがBalloonComponentを持っている場合
                wb::IComponent *balloonComponent = minDistanceEntity->GetComponent(balloon_shooter::BalloonComponentID(), args.componentContainer_);
                if (balloonComponent != nullptr)
                {
                    wb::IComponent *minIdentityComponent = minDistanceEntity->GetComponent(wbp_identity::IdentityComponentID(), args.componentContainer_);
                    wbp_identity::IIdentityComponent *minIdentity = wb::As<wbp_identity::IIdentityComponent>(minIdentityComponent);
                    if (minIdentity == nullptr)
                    {
                        std::string err = wb::CreateErrorMessage
                        (
                            __FILE__, __LINE__, __FUNCTION__,
                            {
                                "BalloonComponentを持つEntityにIdentityComponentがありません。",
                            }
                        );
                        wb::ConsoleLogErr(err);
                        wb::ErrorNotify("WBP_CONTROLLER", err);
                        wb::ThrowRuntimeError(err);
                    }

                    if (minIdentity->IsActiveSelf())
                    {
                        // Balloonのアクティブ
                        minIdentity->SetActiveSelf(false);

                        for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerUIBalloonLeftComponentID()))
                        {
                            wb::IEntity *uiBarEntity = args.entityContainer_.PtrGet(*id);
                            if (uiBarEntity == nullptr) continue;

                            wb::IComponent *uiBalloonLeftComponent = uiBarEntity->GetComponent(balloon_shooter::PlayerUIBalloonLeftComponentID(), args.componentContainer_);
                            balloon_shooter::IUIBarComponent *uiBarBalloonLeft = wb::As<balloon_shooter::IUIBarComponent>(uiBalloonLeftComponent);
                            uiBarBalloonLeft->SetCurrentValue(uiBarBalloonLeft->GetCurrentValue() - 1.0f);

                            if (uiBarBalloonLeft->GetCurrentValue() == 0)
                            {
                                // Balloonが0になった場合、ClearStateに遷移
                                wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
                                balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);
                                if (gameStateSharedFacade == nullptr)
                                {
                                    std::string err = wb::CreateErrorMessage
                                    (
                                        __FILE__, __LINE__, __FUNCTION__,
                                        {
                                            "GameStateSharedFacadeが見つかりません。",
                                        }
                                    );
                                    wb::ConsoleLogErr(err);
                                    wb::ErrorNotify("WBP_CONTROLLER", err);
                                    wb::ThrowRuntimeError(err);
                                }

                                gameStateSharedFacade->SetGameState(balloon_shooter::ClearGameStateID());
                            }
                        }
                    }
                }
            }

            player->SetRayCasted(false);
        }

        if (mouseMonitor != nullptr && mouseMonitor->GetButton(wb::MouseCode::Left) && !player->IsAnimStarted())
        {
            // 射撃アニメーションの開始
            player->SetAnimStarted(true);

            // RayEntityの取得
            wb::IEntity *rayEntity = args.entityContainer_.PtrGet(player->GetRayEntityID());

            wb::IComponent *rayColliderComponent = rayEntity->GetComponent(wbp_collision::RayColliderComponentID(), args.componentContainer_);
            wbp_collision::IRayColliderComponent *rayCollider = wb::As<wbp_collision::IRayColliderComponent>(rayColliderComponent);
            if (rayCollider == nullptr)
            {
                std::string err = wb::CreateErrorMessage
                (
                    __FILE__, __LINE__, __FUNCTION__,
                    {
                        "PlayerComponentにRayColliderComponentがありません。",
                    }
                );
                wb::ConsoleLogErr(err);
                wb::ErrorNotify("WBP_CONTROLLER", err);
                wb::ThrowRuntimeError(err);
            }

            rayCollider->SetCast(true);

            rayCollider->ClearTargetComponentIDs();
            rayCollider->AddTargetComponentID(balloon_shooter::BalloonComponentID());
            rayCollider->AddTargetComponentID(balloon_shooter::MapComponentID());

            wbp_primitive::PrimitiveRay &ray = rayCollider->GetRay();

            ray.SetOrigin(cameraTransform->GetPosition());

            XMFLOAT3 cameraForward;
            XMStoreFloat3(&cameraForward, cameraTransform->GetForward());
            ray.SetDirection(cameraForward);

            ray.SetLength(GUN_RAY_LENGTH);

            player->SetRayCasted(true);
        }

        if (player->IsAnimStarted())
        {
            if (!player->IsRecoilReached())
            {
                player->SetCurrentGunPitch(player->GetCurrentGunPitch() - player->GetRecoilRiseSpeed() * args.deltaTime_);
                if (player->GetCurrentGunPitch() < -player->GetRecoilValue())
                {
                    player->SetRecoilReached(true);
                    player->SetCurrentGunPitch(-player->GetRecoilValue());
                }

                gunTransform->SetLocalRotation(XMFLOAT3(player->GetCurrentGunPitch(), 0.0f, 0.0f));
            }
            else
            {
                player->SetCurrentGunPitch(player->GetCurrentGunPitch() + player->GetMuzzleClimbSpeed() * args.deltaTime_);
                if (player->GetCurrentGunPitch() >= 0.0f)
                {
                    player->SetRecoilReached(false);
                    player->SetAnimStarted(false);
                    player->SetCurrentGunPitch(0.0f);
                }

                gunTransform->SetLocalRotation(XMFLOAT3(player->GetCurrentGunPitch(), 0.0f, 0.0f));
            }
        }

        XMFLOAT3 axis(0.0f, 0.0f, 0.0f);
        float axisInput = 1.0f;
        float moveValue = 0.0f;

        if (keyboardMonitor->GetKey(wb::KeyCode::LShift)) 
        {
            moveValue = player->GetDashSpeed() * args.deltaTime_;
        }
        else
        {
            moveValue = player->GetSpeed() * args.deltaTime_;
        }

        // 通常の移動
        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::W)) axis.z += axisInput;
        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::S)) axis.z -= axisInput;
        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::D)) axis.x += axisInput;
        if (keyboardMonitor != nullptr && keyboardMonitor->GetKey(wb::KeyCode::A)) axis.x -= axisInput;

        wb::IComponent *collisionResultComponent = bodyEntity->GetComponent(wbp_collision::CollisionResultComponentID(), args.componentContainer_);
        wbp_collision::ICollisionResultComponent *collisionResult = wb::As<wbp_collision::ICollisionResultComponent>(collisionResultComponent);
        if (collisionResult == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "PlayerのBodyにCollisionResultComponentがありません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        if (keyboardMonitor->GetKey(wb::KeyCode::Space) && player->IsGrounded())
        {
            player->GetVerticalVelocity() = player->GetJumpPower();
            player->IsGrounded() = false;
            player->GetJumpFrameCount() = 1;
        }

        bool onGround = false;
        for (size_t i = 0; i < collisionResult->GetCollidedCount(); ++i)
        {
            XMFLOAT3 normal = collisionResult->GetCollidedFaceNormal(i);

            if (normal.x == 0.0f && normal.y == 1.0f && normal.z == 0.0f && player->GetJumpFrameCount() == 0)
            {
                onGround = true;
                break;
            }
        }

        if (!onGround && player->IsGrounded())
        {
            player->GetVerticalVelocity() = 0.0f;
            player->IsGrounded() = false;
        }
        
        if (!player->IsGrounded())
        {
            player->GetVerticalVelocity() -= player->GetGravity() * args.deltaTime_;
            axis.y = player->GetVerticalVelocity();
            for (size_t i = 0; i < collisionResult->GetCollidedCount(); ++i)
            {
                XMFLOAT3 normal = collisionResult->GetCollidedFaceNormal(i);

                if (normal.x == 0.0f && normal.y == 1.0f && normal.z == 0.0f && player->GetJumpFrameCount() > 10)
                {
                    // If the collision is with the ground
                    player->IsGrounded() = true;
                    player->GetVerticalVelocity() = 0.0f;

                    break;
                }
            }

            player->GetJumpFrameCount()++;
        }

        if (axis.x != 0.0f || axis.z != 0.0f)
        {
            float length = sqrtf(axis.x * axis.x + axis.z * axis.z);
            axis.x /= length;
            axis.z /= length;

            axis.x *= moveValue;
            axis.z *= moveValue;
        }

        // Yawから回転ベクトルを作成
        XMVECTOR cameraHorizQuatRotVec = XMQuaternionRotationRollPitchYaw
        (
            0.0f, XMConvertToRadians(player->CurrentBodyYaw()), 0.0f
        );

        // BodyのYawに合わせて移動ベクトルを回転
        XMVECTOR axisVec = XMLoadFloat3(&axis);
        axisVec = XMVector3Rotate(axisVec, cameraHorizQuatRotVec);
        XMFLOAT3 convertedAxis;
        XMStoreFloat3(&convertedAxis, axisVec);

        // BodyのYawに合わせてカメラのオフセットを回転
        XMVECTOR cameraOffsetVec = XMLoadFloat3(&player->GetCameraOffset());
        cameraOffsetVec = XMVector3Rotate(cameraOffsetVec, cameraHorizQuatRotVec);
        XMFLOAT3 convertedCameraOffset;
        XMStoreFloat3(&convertedCameraOffset, cameraOffsetVec);

        // 移動ベクトルを設定
        bodyRigidBody->SetVelocity(convertedAxis);

        // Cameraの位置をBodyの位置にオフセットを加えた位置に設定
        cameraTransform->SetLocalPosition
        (
            XMFLOAT3
            (
                bodyTransform->GetPosition().x + convertedCameraOffset.x,
                bodyTransform->GetPosition().y + convertedCameraOffset.y,
                bodyTransform->GetPosition().z + convertedCameraOffset.z
            )
        );

        // マウスの動きでCamera、Bodyを回転させる
        if (mouseMonitor->GetDeltaPositionX() != 0 || mouseMonitor->GetDeltaPositionY() != 0)
        {
            player->CurrentCameraYaw() += mouseMonitor->GetDeltaPositionX() * player->GetSensitivity();
            player->CurrentCameraPitch() += mouseMonitor->GetDeltaPositionY() * player->GetSensitivity();

            // カメラのピッチを制限
            if (player->CurrentCameraPitch() > CAMERA_PITCH_LIMIT)
            {
                player->CurrentCameraPitch() = CAMERA_PITCH_LIMIT;
            }
            else if (player->CurrentCameraPitch() < -CAMERA_PITCH_LIMIT)
            {
                player->CurrentCameraPitch() = -CAMERA_PITCH_LIMIT;
            }

            cameraTransform->SetLocalRotation
            (
                DirectX::XMFLOAT3(player->CurrentCameraPitch(), player->CurrentCameraYaw(), 0.0f)
            );

            // Bodyもカメラと同じ向きに、ただしY軸の回転のみを適用
            player->CurrentBodyYaw() = player->CurrentCameraYaw();

            bodyTransform->SetLocalRotation
            (
                DirectX::XMFLOAT3(0.0f, player->CurrentBodyYaw(), 0.0f)
            );
        }
    }
}

namespace balloon_shooter
{
    WB_REGISTER_SYSTEM(PlayerSystem, PlayerSystemID());

} // namespace balloon_shooter