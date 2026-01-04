#pragma once

#include "riaecs/riaecs.h"
#include "mono_scene/mono_scene.h"

namespace bdc
{
    void CreateInitialEntities(riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont);

    class InitialSystemListFactory : public riaecs::DefaultSystemListFactory
    {
    public:
        InitialSystemListFactory();
        ~InitialSystemListFactory() override;

        virtual std::unique_ptr<riaecs::ISystemList> Create() const override;
    };

} // namespace bdc