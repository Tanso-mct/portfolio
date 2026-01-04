#include "mono_scene/src/pch.h"
#include "mono_scene/include/component_scene_tag.h"

mono_scene::ComponentSceneTag::ComponentSceneTag()
{
}

mono_scene::ComponentSceneTag::~ComponentSceneTag()
{
    sceneEntity_ = riaecs::Entity();
}

void mono_scene::ComponentSceneTag::Setup(SetupParam &param)
{
    sceneEntity_ = param.sceneEntity;
}

MONO_SCENE_API riaecs::ComponentRegistrar
<mono_scene::ComponentSceneTag, mono_scene::ComponentSceneTagMaxCount> mono_scene::ComponentSceneTagID;