#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/title_scene.h"

#include "balloon_shooter/include/scene/title_entities_factory.h"
#include "balloon_shooter/include/scene/title_asset_group.h"
#include "balloon_shooter/include/scene/title_system_scheduler.h"

const size_t &balloon_shooter::TitleSceneFacadeID()
{
    static size_t id = wb::IDFactory::CreateSceneFacadeID();
    return id;
}

namespace balloon_shooter
{
    WB_REGISTER_SCENE_FACADE
    (
        TitleSceneFacadeID,
        TitleEntitiesFactory,
        TitleAssetGroup,
        TitleSystemScheduler
    );
}