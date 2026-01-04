#include "mono_identity_test/pch.h"

#include "mono_identity/include/component_identity.h"
#pragma comment(lib, "mono_identity.lib")
#pragma comment(lib, "riaecs.lib")

TEST(Identity, Component)
{
    mono_identity::ComponentIdentity identity;
    mono_identity::ComponentIdentity::SetupParam param;
    param.name = "Test";
    param.tag = 1;
    param.layer = 2;
    param.activeSelf = false;
    identity.Setup(param);

    EXPECT_EQ(identity.GetName(), "Test");
    EXPECT_EQ(identity.GetTag(), 1);
    EXPECT_EQ(identity.GetLayer(), 2);
    EXPECT_EQ(identity.IsActiveSelf(), false);
}