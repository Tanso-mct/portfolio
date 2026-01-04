#pragma once
#include "mono_transform/include/dll_config.h"
#include "riaecs/riaecs.h"

namespace mono_transform
{
    class MONO_TRANSFORM_API SystemTransform : public riaecs::ISystem
    {
    public:
        SystemTransform();
        ~SystemTransform() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld &ecsWorld, riaecs::IAssetContainer &assetCont, 
            riaecs::ISystemLoopCommandQueue &systemLoopCmdQueue
        ) override;

    private:
        // 何回にかに1回、全てのLastTransformを現在のTransformで更新する
        // その回数を指定する
        int lastTransformUpdateInterval_ = 2;
        int updateCount_ = 0;
    };
    extern MONO_TRANSFORM_API riaecs::SystemFactoryRegistrar<SystemTransform> SystemTransformID;

} // namespace mono_transform