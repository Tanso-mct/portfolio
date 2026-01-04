#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/play_scene.h"

#include "balloon_shooter/include/scene/play_entities_factory.h"
#include "balloon_shooter/include/scene/play_asset_group.h"
#include "balloon_shooter/include/scene/play_system_scheduler.h"

const size_t &balloon_shooter::PlaySceneFacadeID()
{
    static size_t id = wb::IDFactory::CreateSceneFacadeID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_SCENE_FACADE
    (
        PlaySceneFacadeID,
        PlayEntitiesFactory,
        PlayAssetGroup,
        PlaySystemScheduler
    );
}