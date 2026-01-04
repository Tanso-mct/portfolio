#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_ui_title.h"

#include "balloon_shooter/include/feature/component_ui_title.h"

#include "balloon_shooter/include/feature/shared_game_state.h"
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

const size_t &balloon_shooter::TitleUIBackgroundTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::TitleUIPlayBtnTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::TitleUIExitBtnTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        TitleUIBackgroundTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/title_background.png"
    );

    WB_REGISTER_ASSET
    (
        TitleUIPlayBtnTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/title_play_button.png"
    );

    WB_REGISTER_ASSET
    (
        TitleUIExitBtnTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/title_exit_button.png"
    );

} // namespace balloon_shooter

void balloon_shooter::TitleUIPlayBtnEvent::OnClick(const wb::SystemArgument &args)
{
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    gameStateSharedFacade->SetGameState(balloon_shooter::LoadingGameStateID());
}

void balloon_shooter::TitleUIPlayBtnEvent::OnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(hoverScaleAmount_, hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::TitleUIPlayBtnEvent::OnUnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(1.0f / hoverScaleAmount_, 1.0f / hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::TitleUIExitBtnEvent::OnClick(const wb::SystemArgument &args)
{
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    gameStateSharedFacade->SetGameState(balloon_shooter::ExitGameStateID());
}

void balloon_shooter::TitleUIExitBtnEvent::OnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIExitBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(hoverScaleAmount_, hoverScaleAmount_, 1.0f));
    }
}

void balloon_shooter::TitleUIExitBtnEvent::OnUnHover(const wb::SystemArgument &args)
{
    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIExitBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), args.componentContainer_);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->Scale(DirectX::XMFLOAT3(1.0f / hoverScaleAmount_, 1.0f / hoverScaleAmount_, 1.0f));
    }
}

std::unique_ptr<wb::IOptionalValue> balloon_shooter::TitleUIPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> titleUIEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        titleUIEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::TitleUIBackgroundComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*titleUIEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("titleUI");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, 0.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(1920.0f / 2, 1080.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::TitleUIBackgroundTextureAssetID());
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

        entity().AddComponent(balloon_shooter::TitleUIPlayBtnComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*playBtnEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("titleUIPlayBtn");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, -60.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(504.0f / 2, 92.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::TitleUIPlayBtnTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }

        wb::IComponent *uiButtonComponent = entity->GetComponent(balloon_shooter::TitleUIPlayBtnComponentID(), componentCont);
        balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiButtonComponent);
        uiButton->SetEvent(std::make_unique<balloon_shooter::TitleUIPlayBtnEvent>());
        
    }

    std::unique_ptr<wb::IOptionalValue> exitBtnEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        exitBtnEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::TitleUIExitBtnComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*exitBtnEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("titleUIExitBtn");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalPosition(XMFLOAT3(0.0f, -190.0f, 1.0f));
        screenTransform->SetLocalScale(XMFLOAT3(504.0f / 2, 92.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::TitleUIExitBtnTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }

        wb::IComponent *uiButtonComponent = entity->GetComponent(balloon_shooter::TitleUIExitBtnComponentID(), componentCont);
        balloon_shooter::IUIButtonComponent *uiButton = wb::As<balloon_shooter::IUIButtonComponent>(uiButtonComponent);
        uiButton->SetEvent(std::make_unique<balloon_shooter::TitleUIExitBtnEvent>());
    }

    return titleUIEntityId;
}

std::vector<size_t> balloon_shooter::TitleUIPrefab::GetNeedAssetIDs() const
{
    return 
    {
        balloon_shooter::TitleUIBackgroundTextureAssetID(),
        balloon_shooter::TitleUIPlayBtnTextureAssetID(),
        balloon_shooter::TitleUIExitBtnTextureAssetID()
    };
}