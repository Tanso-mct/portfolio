#include "bdc_player/src/pch.h"
#include "bdc_player/include/component_player_enhanced_state.h"

bdc_player::ComponentPlayerEnhancedState::ComponentPlayerEnhancedState()
{
}

bdc_player::ComponentPlayerEnhancedState::~ComponentPlayerEnhancedState()
{
}

void bdc_player::ComponentPlayerEnhancedState::Setup(SetupParam &param)
{
}

BDC_PLAYER_API riaecs::ComponentRegistrar
<bdc_player::ComponentPlayerEnhancedState, bdc_player::ComponentPlayerEnhancedStateMaxCount> bdc_player::ComponentPlayerEnhancedStateID;