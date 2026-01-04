#pragma once
#include "bdc_game_flow/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace bdc_game_flow
{
    class BDC_GAME_FLOW_API SystemGameFlow : public riaecs::ISystem
    {
    public:
        SystemGameFlow();
        ~SystemGameFlow() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern BDC_GAME_FLOW_API riaecs::SystemFactoryRegistrar<SystemGameFlow> SystemGameFlowID;

} // namespace bdc_game_flow