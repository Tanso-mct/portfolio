#include "mono_cycle/src/pch.h"
#include "mono_cycle/include/game_loop.h"

#pragma comment(lib, "riaecs.lib")

mono_cycle::GameLoop::GameLoop(DWORD mainThreadID)
    : mainThreadID_(mainThreadID)
{
}

mono_cycle::GameLoop::~GameLoop()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    gameLoopThread_.get();
}

void mono_cycle::GameLoop::Run(
    riaecs::ISystemLoop &systemLoop, riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont)
{
    {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        if (isRunning_)
            riaecs::NotifyError({"Game loop is already running."}, RIAECS_LOG_LOC);
            
        isRunning_ = true;
    }

    gameLoopThread_ = std::async(std::launch::async, [&systemLoop, &ecsWorld, &assetCont, this]()
    {
        systemLoop.Run(ecsWorld, assetCont);

        {
            std::unique_lock<std::shared_mutex> lock(mutex_);
            isRunning_ = false;

            PostThreadMessage(mainThreadID_, WM_QUIT, 0, 0);
        }
    });
}

bool mono_cycle::GameLoop::IsRunning() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isRunning_;
}