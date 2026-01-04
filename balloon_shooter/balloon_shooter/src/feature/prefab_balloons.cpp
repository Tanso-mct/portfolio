#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_balloons.h"

#include "balloon_shooter/include/feature/component_balloon.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

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

const size_t &balloon_shooter::BalloonModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonLocatorAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonRedTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonBlueTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonGreenTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonYellowTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonPurpleTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::BalloonPinkTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        BalloonModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/balloon/model.fbx"
    );

    WB_REGISTER_ASSET
    (
        BalloonColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/balloon/collider.fbx"
    );

    WB_REGISTER_ASSET
    (
        BalloonLocatorAssetID, wbp_locator::LocatorAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/balloon/locator.fbx"
    );

    WB_REGISTER_ASSET
    (
        BalloonRedTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/red_base_color.png"
    );

    WB_REGISTER_ASSET
    (
        BalloonBlueTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/blue_base_color.png"
    );

    WB_REGISTER_ASSET
    (
        BalloonGreenTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/green_base_color.png"
    );

    WB_REGISTER_ASSET
    (
        BalloonYellowTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/yellow_base_color.png"
    );

    WB_REGISTER_ASSET
    (
        BalloonPurpleTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/purple_base_color.png"
    );

    WB_REGISTER_ASSET
    (
        BalloonPinkTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/balloon/pink_base_color.png"
    );

} // namespace balloon_shooter

std::unique_ptr<wb::IOptionalValue> balloon_shooter::BalloonsPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    // balloonの色の種類ごとのTextureAssetIDの配列を作成
    std::vector<size_t> textureAssetIDs =
    {
        balloon_shooter::BalloonRedTextureAssetID(),
        balloon_shooter::BalloonBlueTextureAssetID(),
        balloon_shooter::BalloonGreenTextureAssetID(),
        balloon_shooter::BalloonYellowTextureAssetID(),
        balloon_shooter::BalloonPurpleTextureAssetID(),
        balloon_shooter::BalloonPinkTextureAssetID(),
    };

    // 乱数生成器を初期化
    std::random_device rd;
    std::mt19937 gen(rd());

    // 色の種類をランダムに選ぶための分布を作成
    std::uniform_int_distribution<> textureDist(0, static_cast<int>(textureAssetIDs.size()) - 1);

    // balloonの移動速度をランダムに設定するための分布を作成
    std::uniform_real_distribution<float> speedDist(5.0f, 15.0f);

    // balloonの移動可能角度をランダムに設定するための分布を作成
    std::uniform_real_distribution<float> angleDist(30.0f, 90.0f);

    {
        wb::LockedRef<wb::IAsset> asset = assetCont.ThreadSafeGet(balloon_shooter::BalloonLocatorAssetID());
        wbp_locator::ILocatorAsset *locatorAsset = wb::As<wbp_locator::ILocatorAsset>(&asset());
        if (locatorAsset == nullptr)
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {
                    "BalloonLocatorAssetが見つかりません。",
                }
            );
            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WBP_PREFAB", err);
            wb::ThrowRuntimeError(err);
        }

        for (size_t i = 0; i < locatorAsset->GetLocates().size(); ++i)
        {
            std::unique_ptr<wb::IOptionalValue> balloonEntityId = nullptr;
            {
                wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
                balloonEntityId = entity().GetID().Clone();

                entity().AddComponent(balloon_shooter::BalloonComponentID(), componentCont);

                entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
                entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
                entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
                entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
                entity().AddComponent(wbp_locator::LocatorComponentID(), componentCont);
            }
            {
                wb::IEntity *entity = entityCont.PtrGet(*balloonEntityId);

                wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
                wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
                identity->SetName("Balloon");

                wb::IComponent *balloonComponent = entity->GetComponent(balloon_shooter::BalloonComponentID(), componentCont);
                balloon_shooter::IBalloonComponent *balloon = wb::As<balloon_shooter::IBalloonComponent>(balloonComponent);
                balloon->SetMovingSpeed(speedDist(gen)); // ランダムな移動速度を設定
                balloon->SetMoveLimit(angleDist(gen)); // ランダムな移動可能角度を設定

                wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
                wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
                meshRenderer->SetModelAssetID(balloon_shooter::BalloonModelAssetID());

                // ランダムに色を選択
                size_t randomIndex = textureDist(gen);
                meshRenderer->AddTextureAssetID(textureAssetIDs[randomIndex], wbp_render::TextureType::Diffuse);
                {
                    wbp_render::ModelColorConfig colorConfig;
                    colorConfig.isColorEnabled = false; // Textureを使用するため、色は無効にする
                    meshRenderer->SetColorConfig(colorConfig);
                }

                wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
                wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
                boxCollider->SetColliderShapeAssetID(balloon_shooter::BalloonColliderShapeAssetID());

                wb::IComponent *locatorComponent = entity->GetComponent(wbp_locator::LocatorComponentID(), componentCont);
                wbp_locator::ILocatorComponent *locator = wb::As<wbp_locator::ILocatorComponent>(locatorComponent);
                locator->SetLocatorAssetID(balloon_shooter::BalloonLocatorAssetID());
                locator->SetLocateTargetIndex(i);
            }
        }
    }

    return nullptr; // 複数のバルーンを生成するため、特定のIDは返さない
};

std::vector<size_t> balloon_shooter::BalloonsPrefab::GetNeedAssetIDs() const
{
    return
    {
        balloon_shooter::BalloonModelAssetID(),
        balloon_shooter::BalloonColliderShapeAssetID(),
        balloon_shooter::BalloonLocatorAssetID(),
        balloon_shooter::BalloonRedTextureAssetID(),
        balloon_shooter::BalloonBlueTextureAssetID(),
        balloon_shooter::BalloonGreenTextureAssetID(),
        balloon_shooter::BalloonYellowTextureAssetID(),
        balloon_shooter::BalloonPurpleTextureAssetID(),
        balloon_shooter::BalloonPinkTextureAssetID(),
    };
}