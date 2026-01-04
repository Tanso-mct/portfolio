#pragma once
#include "mono_cycle/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <future>
#include <mutex>
#include <Windows.h>

namespace mono_cycle
{
    class MONO_CYCLE_API GameLoop
    {
    private:
        bool isRunning_ = false;
        std::future<void> gameLoopThread_;
        mutable std::shared_mutex mutex_;

        const DWORD mainThreadID_;

    public:
        GameLoop(DWORD mainThreadID);
        ~GameLoop();

        void Run(riaecs::ISystemLoop &systemLoop,riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont);
        bool IsRunning() const;
    };

} // namespace mono_cycle