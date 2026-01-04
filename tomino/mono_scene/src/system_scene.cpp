#include "mono_scene/src/pch.h"
#include "mono_scene/include/system_scene.h"

#include "mono_scene/include/component_scene.h"

#pragma comment(lib, "riaecs.lib")

mono_scene::SystemScene::SystemScene()
{
}

mono_scene::SystemScene::~SystemScene()
{
}

bool mono_scene::SystemScene::Update
(
    riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
    riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
){
    for (const riaecs::Entity &entity : ecsWorld.View(mono_scene::ComponentSceneID())())
    {
        mono_scene::ComponentScene *scene
        = riaecs::GetComponent<mono_scene::ComponentScene>(ecsWorld, entity, mono_scene::ComponentSceneID());

        if (scene->NeedsLoad() && riaecs::CheckFutureIsReady(loadFuture))
        {
            loadFuture = std::async
            (
                std::launch::async, &mono_scene::LoadScene, 
                entity, scene, std::ref(ecsWorld), std::ref(assetCont)
            );
        }

        if (scene->IsLoaded() && !scene->StagingEntityAreaRO()().empty())
        {
            // Commit entities
            ecsWorld.CommitEntities(scene->StagingEntityAreaRW()());

            // post create entities
            scene->EntitiesFactoryRO()()->PostCreateEntities(entity, ecsWorld, assetCont);
        }

        if (scene->NeedsRelease())
            mono_scene::ReleaseScene(entity, scene, ecsWorld, assetCont);

        if (scene->NeedsEditSystemList() && scene->IsLoaded())
            AddSystemListEditCommand(scene, systemLoopCmdQueue);
    }

    return true;
}

MONO_SCENE_API riaecs::SystemFactoryRegistrar<mono_scene::SystemScene> mono_scene::SystemSceneID;