#pragma once
#include "bdc_enemy/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_delta_time/mono_delta_time.h"

namespace bdc_enemy
{
    class BDC_ENEMY_API SystemEnemy : public riaecs::ISystem
    {
    private:
        mono_delta_time::DeltaTimeProvider deltaTimeProvider_;
        
    public:
        SystemEnemy();
        ~SystemEnemy() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern BDC_ENEMY_API riaecs::SystemFactoryRegistrar<SystemEnemy> SystemEnemyID;

} // namespace bdc_enemy