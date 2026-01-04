#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_ui_load.h"

#include "balloon_shooter/include/feature/component_ui_load.h"

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

const size_t &balloon_shooter::LoadUIBackgroundTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        LoadUIBackgroundTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/ui/load_background.png"
    );

} // namespace balloon_shooter

std::unique_ptr<wb::IOptionalValue> balloon_shooter::LoadUIPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> loadUIEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        loadUIEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::LoadUIBackgroundComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::SpriteRendererComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*loadUIEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("LoadUI");
        identity->SetActiveSelf(false); // UIは初期状態では非表示

        wb::IComponent *screenTransformComponent = entity->GetComponent(wbp_transform::ScreenTransformComponentID(), componentCont);
        wbp_transform::IScreenTransformComponent *screenTransform = wb::As<wbp_transform::IScreenTransformComponent>(screenTransformComponent);
        screenTransform->SetLocalScale(XMFLOAT3(1920.0f / 2, 1080.0f / 2, 1.0f));

        wb::IComponent *spriteRendererComponent = entity->GetComponent(wbp_render::SpriteRendererComponentID(), componentCont);
        wbp_render::ISpriteRendererComponent *spriteRenderer = wb::As<wbp_render::ISpriteRendererComponent>(spriteRendererComponent);
        spriteRenderer->SetTextureAssetID(balloon_shooter::LoadUIBackgroundTextureAssetID());
        {
            wbp_render::SpriteColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // テクスチャを使用するため、色は無効にする
            spriteRenderer->SetColorConfig(colorConfig);
        }
    }

    return loadUIEntityId;
}

std::vector<size_t> balloon_shooter::LoadUIPrefab::GetNeedAssetIDs() const
{
    return 
    { 
        LoadUIBackgroundTextureAssetID() 
    };
}

