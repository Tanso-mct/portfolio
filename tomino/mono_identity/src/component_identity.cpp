#include "mono_identity/src/pch.h"
#include "mono_identity/include/component_identity.h"

#pragma comment(lib, "riaecs.lib")

mono_identity::ComponentIdentity::ComponentIdentity()
{
}

mono_identity::ComponentIdentity::~ComponentIdentity()
{
    name_.clear();
    tag_ = 0;
    layer_ = 0;
    activeSelf_ = true;
}

void mono_identity::ComponentIdentity::Setup(SetupParam &param)
{
    name_ = param.name;
    tag_ = param.tag;
    layer_ = param.layer;
    activeSelf_ = param.activeSelf;
}

MONO_IDENTITY_API riaecs::ComponentRegistrar
<mono_identity::ComponentIdentity, mono_identity::ComponentIdentityMaxCount> mono_identity::ComponentIdentityID;