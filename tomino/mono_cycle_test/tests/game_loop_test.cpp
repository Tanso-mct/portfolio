#include "mono_cycle_test/pch.h"

#pragma comment(lib, "riaecs.lib")

#include "mem_alloc_fixed_block/mem_alloc_fixed_block.h"
#pragma comment(lib, "mem_alloc_fixed_block.lib")

#include "mono_cycle/include/game_loop.h"
#pragma comment(lib, "mono_cycle.lib")

TEST(GameLoop, Run)
{
    // Create ECS World
    std::unique_ptr<riaecs::IECSWorld> ecsWorld 
        = std::make_unique<riaecs::ECSWorld>(*riaecs::gComponentFactoryRegistry, *riaecs::gComponentMaxCountRegistry);
    ecsWorld->SetPoolFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockPoolFactory>());
    ecsWorld->SetAllocatorFactory(std::make_unique<mem_alloc_fixed_block::FixedBlockAllocatorFactory>());
    ecsWorld->CreateWorld();

    // Create Asset Container
    std::unique_ptr<riaecs::IAssetContainer> assetCont = std::make_unique<riaecs::AssetContainer>();
    assetCont->Create(riaecs::gAssetSourceRegistry->GetCount());

    // Create System Loop
    std::unique_ptr<riaecs::ISystemLoop> systemLoop = std::make_unique<riaecs::SystemLoop>();
    systemLoop->SetSystemListFactory(std::make_unique<riaecs::DefaultSystemListFactory>());
    systemLoop->SetSystemLoopCommandQueueFactory(std::make_unique<riaecs::DefaultSystemLoopCommandQueueFactory>());
    EXPECT_TRUE(systemLoop->IsReady());
    systemLoop->Initialize();

    // Create Game Loop
    std::unique_ptr<mono_cycle::GameLoop> gameLoop = std::make_unique<mono_cycle::GameLoop>();

    // Run Game Loop
    gameLoop->Run(*systemLoop, *ecsWorld, *assetCont);

    // Wait for a short duration to simulate game loop running
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Check if the game loop is running
    EXPECT_FALSE(gameLoop->IsRunning());

    // Cleanup
    gameLoop.reset();
}