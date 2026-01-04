#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/load_scene.h"

#include "balloon_shooter/include/scene/load_entities_factory.h"
#include "balloon_shooter/include/scene/load_asset_group.h"
#include "balloon_shooter/include/scene/load_system_scheduler.h"

const size_t &balloon_shooter::LoadSceneFacadeID()
{
    static size_t id = wb::IDFactory::CreateSceneFacadeID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_SCENE_FACADE
    (
        LoadSceneFacadeID,
        LoadEntitiesFactory,
        LoadAssetGroup,
        LoadSystemScheduler
    );
}