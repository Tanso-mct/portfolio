#pragma once

#include "bdc_player/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_delta_time/mono_delta_time.h"

namespace bdc_player
{
    class BDC_PLAYER_API SystemPlayer : public riaecs::ISystem
    {
    private:
        mono_delta_time::DeltaTimeProvider deltaTimeProvider_;
        
    public:
        SystemPlayer();
        ~SystemPlayer() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern BDC_PLAYER_API riaecs::SystemFactoryRegistrar<SystemPlayer> SystemPlayerID;

} // namespace bdc_player