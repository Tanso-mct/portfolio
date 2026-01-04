#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/play_asset_group.h"

#include "balloon_shooter/include/feature/prefab_player.h"
#include "balloon_shooter/include/feature/prefab_map.h"
#include "balloon_shooter/include/feature/prefab_balloons.h"
#include "balloon_shooter/include/feature/prefab_ui_player.h"
#include "balloon_shooter/include/feature/prefab_ui_clear.h"

balloon_shooter::PlayAssetGroup::PlayAssetGroup()
{
    balloon_shooter::PlayerPrefab playerPrefab;
    for (const size_t &assetID : playerPrefab.GetNeedAssetIDs()) AddAssetID(assetID);

    balloon_shooter::MapPrefab mapPrefab;
    for (const size_t &assetID : mapPrefab.GetNeedAssetIDs()) AddAssetID(assetID);

    balloon_shooter::BalloonsPrefab balloonsPrefab;
    for (const size_t &assetID : balloonsPrefab.GetNeedAssetIDs()) AddAssetID(assetID);

    balloon_shooter::PlayerUIPrefab playerUIPrefab;
    for (const size_t &assetID : playerUIPrefab.GetNeedAssetIDs()) AddAssetID(assetID);

    balloon_shooter::ClearUIPrefab clearUIPrefab;
    for (const size_t &assetID : clearUIPrefab.GetNeedAssetIDs()) AddAssetID(assetID);
}