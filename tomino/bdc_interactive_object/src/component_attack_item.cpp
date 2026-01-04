#include "bdc_interactive_object/src/pch.h"
#include "bdc_interactive_object/include/component_attack_item.h"

bdc_interactive_object::ComponentAttackItem::ComponentAttackItem()
{
}

bdc_interactive_object::ComponentAttackItem::~ComponentAttackItem()
{
}

void bdc_interactive_object::ComponentAttackItem::Setup(SetupParam &param)
{
}

BDC_INTERACTIVE_OBJECT_API riaecs::ComponentRegistrar
<bdc_interactive_object::ComponentAttackItem, bdc_interactive_object::ComponentAttackItemMaxCount> bdc_interactive_object::ComponentAttackItemID;