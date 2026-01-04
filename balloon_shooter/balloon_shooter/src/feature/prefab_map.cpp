#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/prefab_map.h"

#include "balloon_shooter/include/feature/component_map.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_fbx_loader/plugin.h"
#pragma comment(lib, "wbp_fbx_loader.lib")

#include "wbp_png_loader/plugin.h"
#pragma comment(lib, "wbp_png_loader.lib")

#include "wbp_model/plugin.h"
#pragma comment(lib, "wbp_model.lib")

#include "wbp_texture/plugin.h"
#pragma comment(lib, "wbp_texture.lib")

const size_t &balloon_shooter::MapModelAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::MapColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::MapTextureAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

const size_t &balloon_shooter::MapBottomColliderShapeAssetID()
{
    static size_t id = wb::IDFactory::CreateAssetID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_ASSET
    (
        MapModelAssetID, wbp_model::ModelAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/map/model.fbx"
    );

    WB_REGISTER_ASSET
    (
        MapColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/map/map_collider.fbx"
    );

    WB_REGISTER_ASSET
    (
        MapTextureAssetID, wbp_texture::Texture2DAssetFactoryID(), wbp_png_loader::PNGFileLoaderID(),
        "../resources/balloon_shooter/map/base_color.png"
    );

    WB_REGISTER_ASSET
    (
        MapBottomColliderShapeAssetID, wbp_collision::ColliderShapeAssetFactoryID(), wbp_fbx_loader::FBXFileLoaderID(),
        "../resources/balloon_shooter/map/bottom_collider.fbx"
    );

} // namespace balloon_shooter

std::unique_ptr<wb::IOptionalValue> balloon_shooter::MapPrefab::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    std::unique_ptr<wb::IOptionalValue> mapEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        mapEntityId = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        entity().AddComponent(wbp_render::MeshRendererComponentID(), componentCont);
        entity().AddComponent(balloon_shooter::MapComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*mapEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Map");

        wb::IComponent *meshRendererComponent = entity->GetComponent(wbp_render::MeshRendererComponentID(), componentCont);
        wbp_render::IMeshRendererComponent *meshRenderer = wb::As<wbp_render::IMeshRendererComponent>(meshRendererComponent);
        meshRenderer->SetModelAssetID(balloon_shooter::MapModelAssetID());
        meshRenderer->AddTextureAssetID(balloon_shooter::MapTextureAssetID(), wbp_render::TextureType::Diffuse);
        {
            wbp_render::ModelColorConfig colorConfig;
            colorConfig.isColorEnabled = false; // Textureを使用するため、色は無効にする
            meshRenderer->SetColorConfig(colorConfig);
        }

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(balloon_shooter::MapColliderShapeAssetID());
    }

    std::unique_ptr<wb::IOptionalValue> mapBottomEntityId = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        mapBottomEntityId = entity().GetID().Clone();

        entity().AddComponent(balloon_shooter::MapBottomComponentID(), componentCont);

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_collision::BoxColliderComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*mapBottomEntityId);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("MapBottom");

        wb::IComponent *boxColliderComponent = entity->GetComponent(wbp_collision::BoxColliderComponentID(), componentCont);
        wbp_collision::IBoxColliderComponent *boxCollider = wb::As<wbp_collision::IBoxColliderComponent>(boxColliderComponent);
        boxCollider->SetColliderShapeAssetID(balloon_shooter::MapBottomColliderShapeAssetID());
    }

    return mapEntityId;
}

std::vector<size_t> balloon_shooter::MapPrefab::GetNeedAssetIDs() const
{
    return
    {
        balloon_shooter::MapModelAssetID(),
        balloon_shooter::MapColliderShapeAssetID(),
        balloon_shooter::MapTextureAssetID(),
        balloon_shooter::MapBottomColliderShapeAssetID()
    };
}