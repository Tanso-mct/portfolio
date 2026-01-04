#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/play_entities_factory.h"

#include "balloon_shooter/include/feature/prefab_player.h"
#include "balloon_shooter/include/feature/prefab_map.h"
#include "balloon_shooter/include/feature/prefab_balloons.h"
#include "balloon_shooter/include/feature/prefab_ui_player.h"
#include "balloon_shooter/include/feature/prefab_ui_clear.h"

void balloon_shooter::PlayEntitiesFactory::Create
(
    wb::IAssetContainer &assetCont, 
    wb::IEntityContainer &entityCont, 
    wb::IComponentContainer &componentCont, 
    wb::IEntityIDView &entityIDView
) const
{
    balloon_shooter::PlayerPrefab playerPrefab;
    std::unique_ptr<wb::IOptionalValue> playerEntityId = playerPrefab.Create(assetCont, entityCont, componentCont, entityIDView);

    balloon_shooter::MapPrefab mapPrefab;
    std::unique_ptr<wb::IOptionalValue> mapEntityId = mapPrefab.Create(assetCont, entityCont, componentCont, entityIDView);

    balloon_shooter::BalloonsPrefab balloonsPrefab;
    std::unique_ptr<wb::IOptionalValue> balloonsEntityId = balloonsPrefab.Create(assetCont, entityCont, componentCont, entityIDView);

    balloon_shooter::PlayerUIPrefab playerUIPrefab;
    std::unique_ptr<wb::IOptionalValue> playerUIEntityId = playerUIPrefab.Create(assetCont, entityCont, componentCont, entityIDView);

    balloon_shooter::ClearUIPrefab clearUIPrefab;
    std::unique_ptr<wb::IOptionalValue> clearUIEntityId = clearUIPrefab.Create(assetCont, entityCont, componentCont, entityIDView);

}