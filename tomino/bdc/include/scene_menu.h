#pragma once

#include "riaecs/riaecs.h"
#include "mono_scene/mono_scene.h"

namespace bdc
{
    class MenuSceneEntitiesFactory : public mono_scene::IEntitiesFactory
    {
    public:
        MenuSceneEntitiesFactory();
        ~MenuSceneEntitiesFactory() override;

        riaecs::StagingEntityArea CreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
        void PostCreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
    };

    class MenuSceneSystemListEditCmd : public riaecs::ISystemLoopCommand
    {
    public:
        MenuSceneSystemListEditCmd();
        ~MenuSceneSystemListEditCmd() override;

        void Execute(riaecs::ISystemList &systemList, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const override;
        std::unique_ptr<riaecs::ISystemLoopCommand> Clone() const override;
    };

} // namespace bdc