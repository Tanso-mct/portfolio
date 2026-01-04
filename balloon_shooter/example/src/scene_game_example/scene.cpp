#include "example/src/pch.h"
#include "example/include/scene_game_example/scene.h"

#include "example/include/scene_game_example/entities_factory.h"
#include "example/include/scene_game_example/asset_group.h"
#include "example/include/scene_game_example/system_scheduler.h"

const size_t &example::GameExampleSceneFacadeID()
{
    static size_t id = wb::IDFactory::CreateSceneFacadeID();
    return id;
}

namespace example
{
    WB_REGISTER_SCENE_FACADE
    (
        GameExampleSceneFacadeID,
        GameExampleEntitiesFactory,
        GameExampleAssetGroup,
        GameExampleSystemScheduler
    );
}