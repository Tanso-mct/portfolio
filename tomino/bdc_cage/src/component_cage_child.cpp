#include "bdc_cage/src/pch.h"
#include "bdc_cage/include/component_cage_child.h"

bdc_cage::ComponentCageChild::ComponentCageChild()
{
}

bdc_cage::ComponentCageChild::~ComponentCageChild()
{
}

void bdc_cage::ComponentCageChild::Setup(SetupParam &param)
{
}

BDC_CAGE_API riaecs::ComponentRegistrar
<bdc_cage::ComponentCageChild, bdc_cage::ComponentCageChildMaxCount> bdc_cage::ComponentCageChildID;