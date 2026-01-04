#include "bdc_enemy/src/pch.h"
#include "bdc_enemy/include/component_enemy.h"

bdc_enemy::ComponentEnemy::ComponentEnemy()
{
}

bdc_enemy::ComponentEnemy::~ComponentEnemy()
{
}

void bdc_enemy::ComponentEnemy::Setup(SetupParam &param)
{
    wayPoints_ = param.wayPoints;
    goingWayPointIndex_ = param.goingWayPointIndex;
    moveSpeed_ = param.moveSpeed;
}

BDC_ENEMY_API riaecs::ComponentRegistrar
<bdc_enemy::ComponentEnemy, bdc_enemy::ComponentEnemyMaxCount> bdc_enemy::ComponentEnemyID;