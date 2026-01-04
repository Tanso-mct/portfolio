#pragma once

#include "ecs/include/component.h"
#include "mono_scene_extension/include/dll_config.h"
#include "mono_scene_extension/include/scene_tag.h"

namespace mono_scene_extension
{

// The handle class for the component
class MONO_SCENE_EXT_DLL SceneTagComponentHandle : public ecs::ComponentHandle<SceneTagComponentHandle> {};

// The component class
// It contains tags for the scene
class MONO_SCENE_EXT_DLL SceneTagComponent :
    public ecs::Component
{
public:
    SceneTagComponent();
    virtual ~SceneTagComponent() override;

    class SetupParam : //REFLECTABLE_CLASS_BEGIN// 
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() = default;
        virtual ~SetupParam() override = default;

        // The scene ID
        SceneID scene_id = 0;

    }; //REFLECTABLE_CLASS_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;

    // Get the scene ID
    SceneID GetSceneID() const { return scene_id_; }

private:
    // The scene ID
    SceneID scene_id_ = 0;

};

} // namespace mono_scene_extension