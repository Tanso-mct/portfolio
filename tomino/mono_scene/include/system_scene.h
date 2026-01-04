#pragma once
#include "mono_scene/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <future>

namespace mono_scene
{
    class MONO_SCENE_API SystemScene : public riaecs::ISystem
    {
    private:
        std::future<void> loadFuture;

    public:
        SystemScene();
        ~SystemScene() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern MONO_SCENE_API riaecs::SystemFactoryRegistrar<SystemScene> SystemSceneID;

} // namespace mono_scene