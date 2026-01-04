#pragma once

#include "ecs/include/world.h"
#include "mono_scene_extension/include/scene_tag.h"

namespace mono_scene_extension
{

// The interface class for scenes
class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    // Load assets required for the scene
    virtual bool LoadAssets(ecs::World& world) = 0;

    // Check if the assets required for the scene are loaded
    virtual bool IsAssetsLoaded(ecs::World& world) const = 0;

    // Release assets used by the scene
    virtual bool ReleaseAssets(ecs::World& world) = 0;

    // Check if the assets used by the scene are released
    virtual bool IsAssetsReleased(ecs::World& world) const = 0;

    // Create entities required for the scene
    virtual bool Create(ecs::World& world) = 0;

    // Destroy entities used by the scene
    virtual bool Destroy(ecs::World& world) = 0;
};

// The base class for scenes
template <typename SceneType>
class SceneBase :
    public Scene,
    public SceneTag<SceneType>
{
public:
    SceneBase() = default;
    virtual ~SceneBase() override = default;
};

} // namespace mono_scene_extension