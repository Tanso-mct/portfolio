#include "bdc_player/src/pch.h"
#include "bdc_player/include/component_player.h"

bdc_player::ComponentPlayer::ComponentPlayer()
{
}

bdc_player::ComponentPlayer::~ComponentPlayer()
{
}

void bdc_player::ComponentPlayer::Setup(SetupParam &param)
{
    handlerWindowEntity_ = param.handlerWindowEntity;
    modeToggleKey_ = param.modeToggleKey;
    abilityAKey = param.abilityAKey;
    moveSpeed_ = param.moveSpeed;
    enhancedMoveSpeed_ = param.enhancedMoveSpeed;
    blinkDistance_ = param.blinkDistance;
    blinkSpeed_ = param.blinkSpeed;
    normalMaterialHandleID_ = param.normalMaterialHandleID;
    enhancedMaterialHandleID_ = param.enhancedMaterialHandleID;
}

BDC_PLAYER_API riaecs::ComponentRegistrar
<bdc_player::ComponentPlayer, bdc_player::ComponentPlayerMaxCount> bdc_player::ComponentPlayerID;