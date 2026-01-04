#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_ui_clear.h"

#include "balloon_shooter/include/feature/component_ui_clear.h"

#include "balloon_shooter/include/feature/shared_game_state.h"
#include "balloon_shooter/include/feature/game_state_title.h"
#include "balloon_shooter/include/feature/game_state_loading.h"
#include "balloon_shooter/include/feature/game_state_exit.h"

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

const size_t &balloon_shooter::ClearUIBackgroundTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::ClearUIPlayBtnTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::ClearUIMenuBtnTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        ClearUIBackgroundTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/clear_background.png"
    );

    WB_REGISTER_ASSET
    (
        ClearUIPlayBtnTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/clear_play_button.png"
    );

    WB_REGISTER_ASSET
    (
        ClearUIMenuBtnTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/clear_menu_button.png"
    );

} // namespace balloon_shooter

void balloon_shooter::ClearUIPlayBtnEvent::OnClick(const wb::SystemArgument &args)
{
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    gameStateSharedFacade->SetGameState(balloon_shooter::LoadingGameStateID());
}

void balloon_shooter::ClearUIPlayBtnEvent::OnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(hoverScaleAmount_, hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::ClearUIPlayBtnEvent::OnUnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(1.0f / hoverScaleAmount_, 1.0f / hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::ClearUIMenuBtnEvent::OnClick(const wb::SystemArgument &args)
{
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    gameStateSharedFacade->SetGameState(balloon_shooter::TitleGameStateID());
}

void balloon_shooter::ClearUIMenuBtnEvent::OnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIMenuBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(hoverScaleAmount_, hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::ClearUIMenuBtnEvent::OnUnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIMenuBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(1.0f / hoverScaleAmount_, 1.0f / hoverScaleAmount_, 1.0f));
    }
}

std::unique_ptr<wb::IOptionalValue> balloon_shooter::ClearUIPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> clearUIEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        clearUIEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::ClearUIBackgroundComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*clearUIEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("clearUI");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, 0.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(1920.0f / 2, 1080.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::ClearUIBackgroundTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }
    }

    std::unique_ptr<wb::IOptionalValue> playBtnEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        playBtnEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::ClearUIPlayBtnComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*playBtnEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("clearUIPlayBtn");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, -100.0f, 0.9f));
        screenTransform->SetLocalScale(XMFLOAT3(572.0f / 2, 106.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::ClearUIPlayBtnTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }

        wb::IComponent *uiButtonComponent = entity->GetComponent(balloon_shooter::ClearUIPlayBtnComponentID(), componentCont);
        balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiButtonComponent);
        uiButton->SetEvent(std::make_unique<balloon_shooter::ClearUIPlayBtnEvent>());
        
    }

    std::unique_ptr<wb::IOptionalValue> exitBtnEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        exitBtnEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::ClearUIMenuBtnComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*exitBtnEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("clearUIExitBtn");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, -230.0f, 0.9f));
        screenTransform->SetLocalScale(XMFLOAT3(572.0f / 2, 106.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::ClearUIMenuBtnTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }

        wb::IComponent *uiButtonComponent = entity->GetComponent(balloon_shooter::ClearUIMenuBtnComponentID(), componentCont);
        balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiButtonComponent);
        uiButton->SetEvent(std::make_unique<balloon_shooter::ClearUIMenuBtnEvent>());
    }

    return clearUIEntityId;
}

std::vector<size_t> balloon_shooter::ClearUIPrefab::GetNeedAssetIDs() const
{
    return 
    {
        balloon_shooter::ClearUIBackgroundTextureAssetID(),
        balloon_shooter::ClearUIPlayBtnTextureAssetID(),
        balloon_shooter::ClearUIMenuBtnTextureAssetID()
    };
}