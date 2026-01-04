#include "bdc_dot/src/pch.h"
#include "bdc_dot/include/component_dot.h"

bdc_dot::ComponentDot::ComponentDot()
{
}

bdc_dot::ComponentDot::~ComponentDot()
{
}

void bdc_dot::ComponentDot::Setup(SetupParam &param)
{
}

BDC_DOT_API riaecs::ComponentRegistrar
<bdc_dot::ComponentDot, bdc_dot::ComponentDotMaxCount> bdc_dot::ComponentDotID;