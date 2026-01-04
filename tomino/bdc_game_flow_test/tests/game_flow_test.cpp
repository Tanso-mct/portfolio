#include "bdc_game_flow_test/pch.h"

#include "bdc_game_flow/include/component_game_flow.h"
#pragma comment(lib, "bdc_game_flow.lib")

TEST(GameFlow, BasicTransition)
{
    bdc_game_flow::ComponentGameFlow gameFlowComponent;

    bdc_game_flow::ComponentGameFlow::SetupParam param;
    param.needDotCountToSpawnCrystal = 5;
    param.needCrystalCountToClearPhase = 2;
    param.needDeadCountToGameOver = 3;
    gameFlowComponent.Setup(param);

    bdc_game_flow::GameStateMachine& stateMachine = gameFlowComponent.GetStateMachine();
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)bdc_game_flow::GameFlowState::START);

    /*******************************************************************************************************************
     * Handle dot get event in START state
    /******************************************************************************************************************/

    stateMachine.HandleEvent((mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_GET);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)bdc_game_flow::GameFlowState::START);
}

TEST(GameFlow, StartToGameClear)
{
    bdc_game_flow::ComponentGameFlow gameFlowComponent;

    bdc_game_flow::ComponentGameFlow::SetupParam param;
    param.needDotCountToSpawnCrystal = 2;
    param.needCrystalCountToClearPhase = 1;
    param.needDeadCountToGameOver = 3;
    gameFlowComponent.Setup(param);

    bdc_game_flow::GameStateMachine& stateMachine = gameFlowComponent.GetStateMachine();
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)bdc_game_flow::GameFlowState::START);

    /*******************************************************************************************************************
     * Transition to PHASE_1
    /******************************************************************************************************************/

    stateMachine.HandleEvent((mono_cycle::Event)bdc_game_flow::GameFlowEvent::DOT_COLLECT_COUNT_REACHED);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_1);

    /*******************************************************************************************************************
     * Transition to PHASE_2
    /******************************************************************************************************************/

    stateMachine.HandleEvent((mono_cycle::Event)bdc_game_flow::GameFlowEvent::CRYSTAL_COLLECT_COUNT_REACHED);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)bdc_game_flow::GameFlowState::PHASE_2);
}