#include "$project_name$/src/pch.h"
#include "$project_name$/include/scene/$scene_name$_scene.h"

#include "$project_name$/include/scene/$scene_name$_entities_factory.h"
#include "$project_name$/include/scene/$scene_name$_asset_group.h"
#include "$project_name$/include/scene/$scene_name$_system_scheduler.h"

const size_t &$project_name$::$SceneName$SceneFacadeID()
{
    static size_t id = wb::IDFactory::CreateSceneFacadeID();
    return id;
}

namespace $project_name$
{
    WB_REGISTER_SCENE_FACADE
    (
        $SceneName$SceneFacadeID,
        $SceneName$EntitiesFactory,
        $SceneName$AssetGroup,
        $SceneName$SystemScheduler
    );
}