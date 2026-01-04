#pragma once
#include "mono_scene/include/dll_config.h"
#include "riaecs/riaecs.h"

#include <shared_mutex>

namespace mono_scene
{
    constexpr size_t ComponentSceneTagMaxCount = 10000;
    class MONO_SCENE_API ComponentSceneTag
    {
    private:
        riaecs::Entity sceneEntity_;

    public:
        ComponentSceneTag();
        ~ComponentSceneTag();

        struct SetupParam
        {
            riaecs::Entity sceneEntity = riaecs::Entity();
        };
        void Setup(SetupParam &param);

        riaecs::Entity GetSceneEntity() const { return sceneEntity_; }
        void SetSceneEntity(riaecs::Entity entity) { sceneEntity_ = entity; }
    };
    extern MONO_SCENE_API riaecs::ComponentRegistrar<ComponentSceneTag, ComponentSceneTagMaxCount> ComponentSceneTagID;

} // namespace mono_scene