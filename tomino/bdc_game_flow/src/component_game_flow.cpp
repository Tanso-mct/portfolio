#include "bdc_game_flow/src/pch.h"
#include "bdc_game_flow/include/component_game_flow.h"

bdc_game_flow::ComponentGameFlow::ComponentGameFlow()
{
}

bdc_game_flow::ComponentGameFlow::~ComponentGameFlow()
{
}

void bdc_game_flow::ComponentGameFlow::Setup(SetupParam &param)
{
    bdc_game_flow::GameStateMachine &stateMachine = GetStateMachine();

    // Store setup parameters
    needDotCountToSpawnCrystal_ = param.needDotCountToSpawnCrystal;
    needCrystalCountToClearPhase_ = param.needCrystalCountToClearPhase;
    needDeadCountToGameOver_ = param.needDeadCountToGameOver;
}

BDC_GAME_FLOW_API riaecs::ComponentRegistrar
<bdc_game_flow::ComponentGameFlow, bdc_game_flow::ComponentGameFlowMaxCount> bdc_game_flow::ComponentGameFlowID;