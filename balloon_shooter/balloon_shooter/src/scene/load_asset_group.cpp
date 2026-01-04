#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/load_asset_group.h"

#include "balloon_shooter/include/feature/prefab_ui_load.h"

balloon_shooter::LoadAssetGroup::LoadAssetGroup()
{
    balloon_shooter::LoadUIPrefab loadUIPrefab;
    for (const size_t &assetID : loadUIPrefab.GetNeedAssetIDs()) AddAssetID(assetID);
}