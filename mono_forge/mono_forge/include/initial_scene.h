#pragma once

#include "render_graph/include/material_handle.h"

#include "mono_service/include/service_proxy_manager.h"
#include "mono_scene_extension/include/scene.h"

namespace mono_forge
{

class InitialScene : public mono_scene_extension::SceneBase<InitialScene>
{
public:
    InitialScene(
        mono_service::ServiceProxyManager& service_proxy_manager, 
        const ecs::Entity& scene_entity, const ecs::Entity& window_entity);

    InitialScene() = default;
    ~InitialScene() override = default;

    bool LoadAssets(ecs::World& world) override;
    bool IsAssetsLoaded(ecs::World& world) const override;
    bool ReleaseAssets(ecs::World& world) override;
    bool IsAssetsReleased(ecs::World& world) const override;
    bool Create(ecs::World& world) override;
    bool Destroy(ecs::World& world) override;

private:
    bool CreateEntities(ecs::World& world);
    bool CreateMaterials(ecs::World& world);

    bool DestroyEntities(ecs::World& world);
    bool DestroyMaterials(ecs::World& world);

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;

    // The scene entity
    ecs::Entity scene_entity_ = ecs::Entity();

    // The window entity
    ecs::Entity window_entity_ = ecs::Entity();

    // The asset request entity
    ecs::Entity asset_request_entity_ = ecs::Entity();
};


} // namespace mono_forge