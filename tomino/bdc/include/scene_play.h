#pragma once

#include "riaecs/riaecs.h"
#include "mono_scene/mono_scene.h"

namespace bdc
{
    class PlaySceneEntitiesFactory : public mono_scene::IEntitiesFactory
    {
    public:
        PlaySceneEntitiesFactory();
        ~PlaySceneEntitiesFactory() override;

        riaecs::StagingEntityArea CreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
        void PostCreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
    };

    class PlaySceneSystemListEditCmd : public riaecs::ISystemLoopCommand
    {
    public:
        PlaySceneSystemListEditCmd();
        ~PlaySceneSystemListEditCmd() override;

        void Execute(riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
        std::unique_ptr<riaecs::ISystemLoopCommand> Clone() const override;
    };

    class PlaySceneWhenDeadSystemLoopCommand : public riaecs::ISystemLoopCommand
    {
    public:
        PlaySceneWhenDeadSystemLoopCommand() = default;
        ~PlaySceneWhenDeadSystemLoopCommand() override = default;

        void Execute(riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
        std::unique_ptr<riaecs::ISystemLoopCommand> Clone() const override;
    };

} // namespace bdc