#include "bdc/src/pch.h"

#include "bdc/include/init.h"

#pragma comment(lib, "riaecs.lib")
#pragma comment(lib, "mem_alloc_fixed_block.lib")
#pragma comment(lib, "mono_cycle.lib")

#include "mono_service/include/service_registry.h"

int APIENTRY wWinMain
(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow
){
    // Create service id generator
    std::unique_ptr<mono_service::ServiceIDGenerator> service_id_generator 
        = std::make_unique<mono_service::ServiceIDGenerator>();

    /*******************************************************************************************************************
     * Create standard console
    /******************************************************************************************************************/

#ifdef _DEBUG
    riaecs::CreateStandardConsole(L"Debug Console");
#endif

    /*******************************************************************************************************************
     * COM Initialize
    /******************************************************************************************************************/

    HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    if (!SUCCEEDED(hr))
    {
        riaecs::NotifyError({"Failed to initialize COM library."}, RIAECS_LOG_LOC);
        return -1;
    }

    /*******************************************************************************************************************
     * Create ECS World
    /******************************************************************************************************************/

    std::unique_ptr<riaecs::IECSWorld> ecsWorld 
        = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);

    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());

    ecsWorld->CreateWorld();

    /*************************************************  ******************************************************************
     * Create Asset Container
    /******************************************************************************************************************/

    std::unique_ptr<riaecs::IAssetContainer> assetCont = std::make_unique<riaecs::AssetContainer>();
    assetCont->Create(riaecs::gAssetSourceRegistry->GetCount());

    /*******************************************************************************************************************
     * Create System Loop
    /******************************************************************************************************************/

    std::unique_ptr<riaecs::ISystemLoop> systemLoop = std::make_unique<riaecs::SystemLoop>();
    systemLoop->SetSystemListFactory(std::make_unique<bdc::InitialSystemListFactory>());
    systemLoop->SetSystemLoopCommandQueueFactory(std::make_unique<riaecs::DefaultSystemLoopCommandQueueFactory>());
    systemLoop->Initialize();

    /*******************************************************************************************************************
     * Create initial entities
    /******************************************************************************************************************/

    bdc::CreateInitialEntities(*ecsWorld, *assetCont);

    /*******************************************************************************************************************
     * Run game loop
    /******************************************************************************************************************/

    mono_cycle::GameLoop gameLoop(GetCurrentThreadId());
    gameLoop.Run(*systemLoop, *ecsWorld, *assetCont);

    GetCurrentThreadId();

    /*******************************************************************************************************************
     * Run message loop
    /******************************************************************************************************************/

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    /*******************************************************************************************************************
     * Cleanup
    /******************************************************************************************************************/

    ecsWorld.reset();
    assetCont.reset();
    CoUninitialize();

    return 0;
}