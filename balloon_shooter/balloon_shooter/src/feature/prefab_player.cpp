#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_player.h"

#include "balloon_shooter/include/feature/component_player.h"

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

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

#include "wbp_png_loader/plugin.h"
#pragma comment(lib, "wbp_png_loader.lib")

#include "wbp_model/plugin.h"
#pragma comment(lib, "wbp_model.lib")

#include "wbp_texture/plugin.h"
#pragma comment(lib, "wbp_texture.lib")

const size_t &balloon_shooter::PlayerColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::PlayerLocatorAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::CameraLocatorAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::GunModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        PlayerColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/player/player_collider.fbx"
    );

    WB_REGISTER_ASSET
    (
        PlayerLocatorAssetID, wbp_locator::LocatorAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/player/player_locator.fbx"
    );

    WB_REGISTER_ASSET
    (
        CameraLocatorAssetID, wbp_locator::LocatorAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/player/camera_locator.fbx"
    );

    WB_REGISTER_ASSET
    (
        GunModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/gun/model.fbx"
    );

} // namespace balloon_shooter

std::unique_ptr<wb::IOptionalValue> balloon_shooter::PlayerPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> playerEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        playerEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::PlayerComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> cameraEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityId = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
        entity().AddComponent(wbp_locator::LocatorComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> bodyEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        bodyEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        entity().AddComponent(wbp_collision::CollisionResultComponentID(), componentCont);
        entity().AddComponent(wbp_physics::RigidBodyComponentID(), componentCont);
        entity().AddComponent(wbp_locator::LocatorComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> gunEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        gunEntityId = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> rayEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        rayEntityId = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_collision::RayColliderComponentID(), componentCont);
        entity().AddComponent(wbp_collision::CollisionResultComponentID(), componentCont);
    }

    // Player Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*playerEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Player");

        wb::IComponent *playerComponent = entity->GetComponent(balloon_shooter::PlayerComponentID(), componentCont);
        balloon_shooter::IPlayerComponent *player = wb::As<balloon_shooter::IPlayerComponent>(playerComponent);
        player->GetSpeed() = 100.0f; // 移動速度の設定
        player->GetSensitivity() = 0.1f; // カメラの感度の設定

        player->GetDashSpeed() = 200.0f; // ダッシュ速度の設定
        player->GetJumpPower() = 2.0f; // ジャンプ力の設定
        player->GetGravity() = 6.0f; // 重力の設定

        player->SetRecoilValue(10.0f); // リコイル値の設定
        player->SetRecoilRiseSpeed(90.0f); // リコイルの上昇速度の設定
        player->SetMuzzleClimbSpeed(60.0f); // マズルのクライム速度の設定

        player->SetBodyEntityID(bodyEntityID->Clone());
        player->SetCameraEntityID(cameraEntityId->Clone());
        player->SetGunEntityID(gunEntityId->Clone());
        player->SetRayEntityID(rayEntityId->Clone());
    }

    // Camera Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Camera");

        wb::IComponent *locatorComponent = entity->GetComponent(wbp_locator::LocatorComponentID(), componentCont);
        wbp_locator::ILocatorComponent *locator = wb::As<wbp_locator::ILocatorComponent>(locatorComponent);
        locator->SetLocatorAssetID(balloon_shooter::CameraLocatorAssetID());

        wb::IComponent *cameraComponent = entity->GetComponent(wbp_render::CameraComponentID(), componentCont);
        wbp_render::ICameraComponent *camera = wb::As<wbp_render::ICameraComponent>(cameraComponent);
        camera->SetFarZ(100000.0f); // カメラの遠方クリップ面を設定
        camera->SetFieldOfView(60.0f); // カメラの視野角を設定
    }

    // Body Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*bodyEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Body");

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(balloon_shooter::PlayerColliderShapeAssetID());
        boxCollider->SetAlwaysRunnerEnabled(true); // 常にRunnerとして扱う

        wb::IComponent *rigidBodyComponent = entity->GetComponent(wbp_physics::RigidBodyComponentID(), componentCont);
        wbp_physics::IRigidBodyComponent *rigidBody = wb::As<wbp_physics::IRigidBodyComponent>(rigidBodyComponent);
        rigidBody->SetKinematic(true);

        wb::IComponent *locatorComponent = entity->GetComponent(wbp_locator::LocatorComponentID(), componentCont);
        wbp_locator::ILocatorComponent *locator = wb::As<wbp_locator::ILocatorComponent>(locatorComponent);
        locator->SetLocatorAssetID(balloon_shooter::PlayerLocatorAssetID());
    }

    // Gun Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*gunEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Gun");

        wb::IComponent *transformComponent = entity->GetComponent(wbp_transform::TransformComponentID(), componentCont);
        wbp_transform::ITransformComponent *transform = wb::As<wbp_transform::ITransformComponent>(transformComponent);
        transform->SetParent(entity, entityCont.PtrGet(*cameraEntityId), entityCont, componentCont);

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(balloon_shooter::GunModelAssetID());
        {
            wbp_render::ModelColorConfig colorConfig;
            colorConfig.isColorEnabled = true; // 色を有効にする
            meshRenderer->SetColorConfig(colorConfig);
        }
    }

    // Ray Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*rayEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Gun_Ray");
    }

    return playerEntityId;
}

std::vector<size_t> balloon_shooter::PlayerPrefab::GetNeedAssetIDs() const
{
    return
    {
        balloon_shooter::PlayerColliderShapeAssetID(),
        balloon_shooter::PlayerLocatorAssetID(),
        balloon_shooter::CameraLocatorAssetID(),
        balloon_shooter::GunModelAssetID(),
    };
}