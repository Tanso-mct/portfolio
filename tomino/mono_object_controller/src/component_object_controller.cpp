#include "mono_object_controller/src/pch.h"
#include "mono_object_controller/include/component_object_controller.h"

mono_object_controller::ComponentObjectController::ComponentObjectController()
{
}

mono_object_controller::ComponentObjectController::~ComponentObjectController()
{
}

void mono_object_controller::ComponentObjectController::Setup(SetupParam &param)
{
    handlerWindowEntity_ = param.handlerWindowEntity;

    forwardKey_ = param.forwardKey;
    backKey_ = param.backKey;
    leftKey_ = param.leftKey;
    rightKey_ = param.rightKey;

    moveSpeed_ = param.moveSpeed;
}

MONO_OBJECT_CONTROLLER_API riaecs::ComponentRegistrar
<mono_object_controller::ComponentObjectController, mono_object_controller::ComponentObjectControllerMaxCount> mono_object_controller::ComponentObjectControllerID;