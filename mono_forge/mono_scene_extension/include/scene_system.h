#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"

#include "mono_scene_extension/include/dll_config.h"
#include "mono_scene_extension/include/scene_tag.h"

namespace mono_scene_extension
{

// The handle class for the system
class MONO_SCENE_EXT_DLL SceneSystemHandle :
    public ecs::SystemHandle<SceneSystemHandle> {};

// The system class
class MONO_SCENE_EXT_DLL SceneSystem :
    public ecs::System
{
public:
    SceneSystem();
    virtual ~SceneSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The singleton scene ID generator
    std::unique_ptr<SceneIDGenerator> scene_id_generator_ = nullptr;

};


} // namespace mono_scene_extension