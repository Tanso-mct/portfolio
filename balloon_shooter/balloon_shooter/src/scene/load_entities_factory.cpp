#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/load_entities_factory.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "balloon_shooter/include/feature/prefab_ui_load.h"

void balloon_shooter::LoadEntitiesFactory::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    // Camera Entityの作成
    std::unique_ptr<wb::IOptionalValue> cameraEntityID = nullptr;
    {
        wb::CreatingEntity entity = wb::CreateEntity(entityCont, entityIDView);
        cameraEntityID = entity().GetID().Clone();

        entity().AddComponent(wbp_identity::IdentityComponentID(), componentCont);
        entity().AddComponent(wbp_transform::TransformComponentID(), componentCont);
        entity().AddComponent(wbp_render::CameraComponentID(), componentCont);
    }
    {
        wb::IEntity *entity = entityCont.PtrGet(*cameraEntityID);

        wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
        wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
        identity->SetName("Camera");
    }

    balloon_shooter::LoadUIPrefab loadUIPrefab;
    std::unique_ptr<wb::IOptionalValue> loadUIEntityId = loadUIPrefab.Create(assetCont, entityCont, componentCont, entityIDView);
}