#include "bdc/src/pch.h"
#include "bdc/include/init.h"

#include "bdc/include/entities.h"

void bdc::CreateInitialEntities(riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont)
{
    riaecs::Entity menuSceneEntity = CreateMenuScene(ecsWorld, assetCont);
    riaecs::Entity playSceneEntity = CreatePlayScene(ecsWorld, assetCont);

    riaecs::Entity mainWindowEntity = CreateMainWindow(ecsWorld, assetCont, menuSceneEntity);
}

bdc::InitialSystemListFactory::InitialSystemListFactory()
{
}

bdc::InitialSystemListFactory::~InitialSystemListFactory()
{
}

std::unique_ptr<riaecs::ISystemList> bdc::InitialSystemListFactory::Create() const
{
    // Create system list
    std::unique_ptr<riaecs::ISystemList> systemList = std::make_unique<riaecs::SystemList>();

    // Add systems

    systemList->CreateSystem(mono_d3d12::SystemWindowD3D12ID());
    systemList->CreateSystem(mono_scene::SystemSceneID());
    systemList->CreateSystem(mono_render::SystemRenderID());

    // Set system update order
    systemList->SetOrder
    ({ 
        mono_d3d12::SystemWindowD3D12ID(), 
        mono_scene::SystemSceneID(),
        mono_render::SystemRenderID()
    });

    return systemList;
}