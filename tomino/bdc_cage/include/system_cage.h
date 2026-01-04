#pragma once
#include "bdc_cage/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_transform/include/component_transform.h"
#include "bdc_cage/include/component_cage.h"
#include "mono_delta_time/include/delta_time_provider.h"



namespace bdc_cage
{
    enum class GridDirection
    {
        North, // +Z•ûŒü
        South, // -Z•ûŒü
        East,  // +X•ûŒü
        West,   // -X•ûŒü
        none
    };

    BDC_CAGE_API GridDirection DecideNextDirection(bdc_cage::ComponentCage* cageComp, DirectX::XMFLOAT3 NowPos, float StageSize);

    BDC_CAGE_API DirectX::XMFLOAT3 MoveInDirection(
        mono_transform::ComponentTransform* transform,
        bdc_cage::GridDirection dir,
        float speed,
        float dt);

    BDC_CAGE_API DirectX::XMFLOAT3 updateCagePos(
        bdc_cage::ComponentCage* cageComp,
        mono_transform::ComponentTransform* transform,
        GridDirection& cageMoveDir,
        float stageSize,
        float dt);

    BDC_CAGE_API DirectX::XMFLOAT3 GetGridByPosition(DirectX::XMFLOAT3 targetPos,float StageSize, float cageSize,int numDivisions);

    BDC_CAGE_API bool ReachedTarget(
        DirectX::XMFLOAT3 prevPos,
        DirectX::XMFLOAT3 nowPos,
        float StageSize,
        float cageSize,
        int numDivisions);

    constexpr const float STAGE_SIZE = 96.0f;

    class BDC_CAGE_API SystemCage : public riaecs::ISystem
    {
    public:
        SystemCage();
        ~SystemCage() override;

        /***************************************************************************************************************
         * ISystem Implementation
        /**************************************************************************************************************/

        bool Update
        (
            riaecs::IECSWorld& ecsWorld, riaecs::IAssetContainer& assetCont,
            riaecs::ISystemLoopCommandQueue& systemLoopCmdQueue
        ) override;

	private:
        mono_delta_time::DeltaTimeProvider deltaTimeProvider_;
        GridDirection cageMoveDir_;
    };
    extern BDC_CAGE_API riaecs::SystemFactoryRegistrar<SystemCage> SystemCageID;

} // namespace $project_name$