#include "bdc_interactive_object/src/pch.h"
#include "bdc_interactive_object/include/component_crystal.h"

bdc_interactive_object::ComponentCrystal::ComponentCrystal()
{
}

bdc_interactive_object::ComponentCrystal::~ComponentCrystal()
{
}

void bdc_interactive_object::ComponentCrystal::Setup(SetupParam &param)
{
}

BDC_INTERACTIVE_OBJECT_API riaecs::ComponentRegistrar
<bdc_interactive_object::ComponentCrystal, bdc_interactive_object::ComponentCrystalMaxCount> bdc_interactive_object::ComponentCrystalID;