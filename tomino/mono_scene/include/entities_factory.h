#pragma once
#include "mono_scene/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace mono_scene
{
    class IEntitiesFactory
    {
    public:
        virtual ~IEntitiesFactory() = default;
        
        virtual riaecs::StagingEntityArea CreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const = 0;

        virtual void PostCreateEntities(
            riaecs::Entity sceneEntity, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont) const
        {
            // Default implementation does nothing
        }
    };

} // namespace mono_scene