#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_ui_player.h"

#include "balloon_shooter/include/feature/component_ui_player.h"

using namespace DirectX;

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_png_loader/plugin.h"
#pragma comment(lib, "wbp_png_loader.lib")

#include "wbp_texture/plugin.h"
#pragma comment(lib, "wbp_texture.lib")

#include "wbp_locator/plugin.h"
#pragma comment(lib, "wbp_locator.lib")

#include "balloon_shooter/include/feature/prefab_balloons.h"

const size_t &balloon_shooter::PlayerUITextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        PlayerUITextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/player_ui.png"
    );

} // namespace balloon_shooter

std::unique_ptr<wb::IOptionalValue> balloon_shooter::PlayerUIPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> playerUIEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        playerUIEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::PlayerUIComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }

    {
        wb::IEntity *entity = entityCont.PtrGet(*playerUIEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("PlayerUI");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, 0.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(1920.0f / 2, 1080.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::PlayerUITextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }
    }

    std::unique_ptr<wb::IOptionalValue> balloonLeftBarEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        balloonLeftBarEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::PlayerUIBalloonLeftComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> frameEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        frameEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }

    std::unique_ptr<wb::IOptionalValue> barEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        barEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }

    // balloonLeftBarEntityIdの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*balloonLeftBarEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("balloonLeftBar");
        identity->SetActiveSelf(false); // UIは初期状態では非表示
        
        wb::IComponent *playerUIBalloonLeftBarComponent = entity->GetComponent(balloon_shooter::PlayerUIBalloonLeftComponentID(), componentCont);
        balloon_shooter::IUIBarComponent *uiBar = wb::As<balloon_shooter::IUIBarComponent>(playerUIBalloonLeftBarComponent);

        uiBar->SetFrameEntityID(frameEntityID->Clone());
        uiBar->SetFrameColor(XMFLOAT4(84.0f / 256.0f, 84.0f / 256.0f, 84.0f / 256.0f, 1.0f));

        uiBar->SetBarEntityID(barEntityID->Clone());
        uiBar->SetBarColor(XMFLOAT4(63.0f / 256.0f, 38.0f / 256.0f, 126.0f / 256.0f, 1.0f));

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
            wb::ErrorNotify("WBP_CONTROLLER", err);
            wb::ThrowRuntimeError(err);
        }

        uiBar->SetMaxValue(locatorAsset->GetLocates().size());
        uiBar->SetCurrentValue(locatorAsset->GetLocates().size());
        uiBar->SetMinValue(0.0f);
    }

    // Frame Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*frameEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("BalloonLeftBarFrame");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, 450.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(880.0f, 30.0f, 1.0f));
    }

    // Body Entityの初期化
    {
        wb::IEntity *entity = entityCont.PtrGet(*barEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("BalloonLeftBarBody");
        identity->SetActiveSelf(false); // UIは初期状態では非表示
    }

    return playerUIEntityId;
}

std::vector<size_t> balloon_shooter::PlayerUIPrefab::GetNeedAssetIDs() const
{
    return 
    { 
        balloon_shooter::PlayerUITextureAssetID() 
    };
}