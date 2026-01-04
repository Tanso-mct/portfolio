#include "mono_scene_extension_test/pch.h"

using namespace DirectX;

#include "ecs/include/world.h"

#include "mono_scene_extension/include/allocator_factory.h"
#include "mono_scene_extension/include/scene_component.h"
#include "mono_scene_extension/include/scene.h"

namespace mono_scene_extension_test
{

class TestScene : public mono_scene_extension::SceneBase<TestScene>
{
public:
    TestScene() = default;
    virtual ~TestScene() override = default;

    bool LoadAssets(ecs::World& world) override { return true; }
    bool IsAssetsLoaded(ecs::World& world) const override { return true; }
    bool ReleaseAssets(ecs::World& world) override { return true; }
    bool IsAssetsReleased(ecs::World& world) const override { return true; }
    bool Create(ecs::World& world) override { return true; }
    bool Destroy(ecs::World& world) override { return true; }
};

} // namespace mono_scene_extension_test

TEST(SceneComponent, Accessors)
{
    bool result = false;

    /*******************************************************************************************************************
     * Create ECS world
    /******************************************************************************************************************/

    
    std::unique_ptr<ecs::ComponentIDGenerator> component_id_generator
        = std::make_unique<ecs::ComponentIDGenerator>();

    std::unique_ptr<ecs::World> ecs_world = nullptr;
    {
        // Create component descriptor registry
        std::unique_ptr<ecs::ComponentDescriptorRegistry> component_descriptor_registry
            = std::make_unique<ecs::ComponentDescriptorRegistry>();

        // Register SceneComponent
        constexpr size_t MAX_SCENE_COMPONENT_COUNT = 10;
        {
            std::unique_ptr<ecs::ComponentDescriptor> scene_component_desc
                = std::make_unique<ecs::ComponentDescriptor>(
                    sizeof(mono_scene_extension::SceneComponent),
                    MAX_SCENE_COMPONENT_COUNT,
                    std::make_unique<mono_scene_extension::ComponentAllocatorFactory>());

            component_descriptor_registry->WithUniqueLock([&](ecs::ComponentDescriptorRegistry& registry)
            {
                // Register the descriptor with component ID
                registry.Register(
                    mono_scene_extension::SceneComponentHandle::ID(), std::move(scene_component_desc));
            });
        }

        // Create the ecs world
        ecs_world = std::make_unique<ecs::World>(std::move(component_descriptor_registry));
    }

    /*******************************************************************************************************************
     * Create entity and add SceneComponent
    /******************************************************************************************************************/

    ecs::Entity entity = ecs_world->CreateEntity();

    {
        // Create SceneComponent setup parameters
        std::unique_ptr<mono_scene_extension::SceneComponent::SetupParam> param
            = std::make_unique<mono_scene_extension::SceneComponent::SetupParam>();

        param->required_window_component_ids = { 1 };
        param->required_camera_component_ids = { 1 };
        param->required_renderable_component_ids = { 1 };
        param->required_ui_component_ids = { 1 };
        param->light_component_ids = { 1 };

        // Create TestScene instance
        std::unique_ptr<mono_scene_extension_test::TestScene> scene
            = std::make_unique<mono_scene_extension_test::TestScene>();

        // Add SceneComponent to entity
        result = ecs_world->AddComponent<mono_scene_extension::SceneComponent>(
            entity, mono_scene_extension::SceneComponentHandle::ID(), std::move(param), 
            mono_scene_extension::SceneID(1), std::move(scene));
        ASSERT_TRUE(result);
    }
}

