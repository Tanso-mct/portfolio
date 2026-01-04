#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/title_asset_group.h"

#include "balloon_shooter/include/feature/prefab_ui_title.h"

balloon_shooter::TitleAssetGroup::TitleAssetGroup()
{
    balloon_shooter::TitleUIPrefab titleUIPrefab;
    for (const size_t &assetID : titleUIPrefab.GetNeedAssetIDs()) AddAssetID(assetID);
}