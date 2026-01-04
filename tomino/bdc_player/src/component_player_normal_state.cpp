#include "bdc_player/src/pch.h"
#include "bdc_player/include/component_player_normal_state.h"

bdc_player::ComponentPlayerNormalState::ComponentPlayerNormalState()
{
}

bdc_player::ComponentPlayerNormalState::~ComponentPlayerNormalState()
{
}

void bdc_player::ComponentPlayerNormalState::Setup(SetupParam &param)
{
}

BDC_PLAYER_API riaecs::ComponentRegistrar
<bdc_player::ComponentPlayerNormalState, bdc_player::ComponentPlayerNormalStateMaxCount> bdc_player::ComponentPlayerNormalStateID;