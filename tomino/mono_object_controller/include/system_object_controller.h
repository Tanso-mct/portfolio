#pragma once
#include "mono_object_controller/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_delta_time/mono_delta_time.h"

namespace mono_object_controller
{
    class MONO_OBJECT_CONTROLLER_API SystemObjectController : public riaecs::ISystem
    {
    private:
        mono_delta_time::DeltaTimeProvider deltaTimeProvider_;
        
    public:
        SystemObjectController();
        ~SystemObjectController() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;
    };
    extern MONO_OBJECT_CONTROLLER_API riaecs::SystemFactoryRegistrar<SystemObjectController> SystemObjectControllerID;

} // namespace mono_object_controller