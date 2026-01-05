#include "mono_forge_app_template/src/pch.h"
#include "mono_forge_app_template/include/component_adders.h"

#include "mono_forge_app_template/include/assets.h"
#include "mono_forge_app_template/include/materials.h"

#include "mono_asset_service/include/asset_service.h"
#include "mono_asset_service/include/asset_service_view.h"
#include "mono_asset_extension/include/mesh_asset.h"

#include "mono_meta_extension/include/meta_component.h"

#include "mono_transform_service/include/transform_service.h"
#include "mono_transform_extension/include/transform_component.h"

#include "mono_graphics_service/include/graphics_service.h"
#include "mono_graphics_extension/include/camera_component.h"
#include "mono_graphics_extension/include/directional_light_component.h"
#include "mono_graphics_extension/include/ambient_light_component.h"
#include "mono_graphics_extension/include/point_light_component.h"
#include "mono_graphics_extension/include/renderable_component.h"
#include "mono_graphics_extension/include/graphics_system.h"

namespace mono_forge_app_template
{

bool MetaComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get setup param for MetaComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add MetaComponent to the world for the specified entity
    return world.AddComponent<mono_meta_extension::MetaComponent>(
        entity, mono_meta_extension::MetaComponentHandle::ID(),
        std::move(param));
}

bool MetaComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Add MetaComponent to the world for the specified entity
    return world.AddComponent<mono_meta_extension::MetaComponent>(
        entity, mono_meta_extension::MetaComponentHandle::ID(),
        std::move(setup_param));
}

std::unique_ptr<ecs::Component::SetupParam> MetaComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for MetaComponent
    std::unique_ptr<mono_meta_extension::MetaComponent::SetupParam> param
        = std::make_unique<mono_meta_extension::MetaComponent::SetupParam>();

    return param;
}

bool TransformComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        // Get transform service proxy
        transform_service_proxy = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID()).Clone();
    });

    // Get setup param for TransformComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add TransformComponent to the world for the specified entity
    return world.AddComponent<mono_transform_extension::TransformComponent>(
        entity, mono_transform_extension::TransformComponentHandle::ID(),
        std::move(param), std::move(transform_service_proxy));
}

bool TransformComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    std::unique_ptr<mono_service::ServiceProxy> transform_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        // Get transform service proxy
        transform_service_proxy = manager.GetServiceProxy(mono_transform_service::TransformServiceHandle::ID()).Clone();
    });

    // Add TransformComponent to the world for the specified entity
    return world.AddComponent<mono_transform_extension::TransformComponent>(
        entity, mono_transform_extension::TransformComponentHandle::ID(),
        std::move(setup_param), std::move(transform_service_proxy));
}

std::unique_ptr<ecs::Component::SetupParam> TransformComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for TransformComponent
    std::unique_ptr<mono_transform_extension::TransformComponent::SetupParam> param
        = std::make_unique<mono_transform_extension::TransformComponent::SetupParam>();

    return param;
}

bool CameraComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Get setup param for CameraComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add CameraComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::CameraComponent>(
        entity, mono_graphics_extension::CameraComponentHandle::ID(),
        std::move(param), std::move(graphics_service_proxy));
}

bool CameraComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Add CameraComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::CameraComponent>(
        entity, mono_graphics_extension::CameraComponentHandle::ID(),
        std::move(setup_param), std::move(graphics_service_proxy));
}

std::unique_ptr<ecs::Component::SetupParam> CameraComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for CameraComponent
    std::unique_ptr<mono_graphics_extension::CameraComponent::SetupParam> param
        = std::make_unique<mono_graphics_extension::CameraComponent::SetupParam>();

    return param;
}

bool DirectionalLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Get setup param for DirectionalLightComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add DirectionalLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::DirectionalLightComponent>(
        entity, mono_graphics_extension::DirectionalLightComponentHandle::ID(),
        std::move(param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

bool DirectionalLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Add DirectionalLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::DirectionalLightComponent>(
        entity, mono_graphics_extension::DirectionalLightComponentHandle::ID(),
        std::move(setup_param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

std::unique_ptr<ecs::Component::SetupParam> DirectionalLightComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for DirectionalLightComponent
    std::unique_ptr<mono_graphics_extension::DirectionalLightComponent::SetupParam> param
        = std::make_unique<mono_graphics_extension::DirectionalLightComponent::SetupParam>();

    return param;
}

bool AmbientLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Get setup param for AmbientLightComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add AmbientLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::AmbientLightComponent>(
        entity, mono_graphics_extension::AmbientLightComponentHandle::ID(),
        std::move(param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

bool AmbientLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Add AmbientLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::AmbientLightComponent>(
        entity, mono_graphics_extension::AmbientLightComponentHandle::ID(),
        std::move(setup_param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

std::unique_ptr<ecs::Component::SetupParam> AmbientLightComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for AmbientLightComponent
    std::unique_ptr<mono_graphics_extension::AmbientLightComponent::SetupParam> param
        = std::make_unique<mono_graphics_extension::AmbientLightComponent::SetupParam>();

    return param;
}

bool PointLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Get setup param for PointLightComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);

    // Add PointLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::PointLightComponent>(
        entity, mono_graphics_extension::PointLightComponentHandle::ID(),
        std::move(param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

bool PointLightComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Add PointLightComponent to the world for the specified entity
    return world.AddComponent<mono_graphics_extension::PointLightComponent>(
        entity, mono_graphics_extension::PointLightComponentHandle::ID(),
        std::move(setup_param), std::move(graphics_service_proxy), mono_graphics_extension::BACK_BUFFER_COUNT);
}

std::unique_ptr<ecs::Component::SetupParam> PointLightComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for PointLightComponent
    std::unique_ptr<mono_graphics_extension::PointLightComponent::SetupParam> param
        = std::make_unique<mono_graphics_extension::PointLightComponent::SetupParam>();

    return param;
}

bool RenderableComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics and asset service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    // Get setup param for RenderableComponent
    std::unique_ptr<ecs::Component::SetupParam> param = GetSetupParam(service_proxy_manager);
    
    return world.AddComponent<mono_graphics_extension::RenderableComponent>(
        entity, mono_graphics_extension::RenderableComponentHandle::ID(),
        std::move(param), std::move(graphics_service_proxy));
}

bool RenderableComponentAdder::Add(
    ecs::World &world, const ecs::Entity &entity,
    std::unique_ptr<ecs::Component::SetupParam> setup_param,
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Get graphics and asset service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
    });

    return world.AddComponent<mono_graphics_extension::RenderableComponent>(
        entity, mono_graphics_extension::RenderableComponentHandle::ID(),
        std::move(setup_param), std::move(graphics_service_proxy));
}

std::unique_ptr<ecs::Component::SetupParam> RenderableComponentAdder::GetSetupParam(
    mono_service::ServiceProxyManager& service_proxy_manager) const
{
    // Create default setup param for RenderableComponent
    std::unique_ptr<mono_graphics_extension::RenderableComponent::SetupParam> param
        = std::make_unique<mono_graphics_extension::RenderableComponent::SetupParam>();

    // Get graphics and asset service proxies
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy = nullptr;
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
    service_proxy_manager.WithLock([&](mono_service::ServiceProxyManager& manager)
    {
        graphics_service_proxy = manager.GetServiceProxy(mono_graphics_service::GraphicsServiceHandle::ID()).Clone();
        asset_service_proxy = manager.GetServiceProxy(mono_asset_service::AssetServiceHandle::ID()).Clone();
    });

    // Create asset service view
    std::unique_ptr<mono_service::ServiceView> asset_service_view_ptr = asset_service_proxy->CreateView();
    mono_asset_service::AssetServiceView* asset_service_view
        = dynamic_cast<mono_asset_service::AssetServiceView*>(asset_service_view_ptr.get());
    assert(asset_service_view != nullptr && "Asset service view is null!");

    // Get box mesh asset
    const asset_loader::Asset& mesh_asset
        = asset_service_view->GetAsset(BoxMeshAssetHandle::ID());
    const mono_asset_extension::MeshAsset* mesh_asset_ptr
        = dynamic_cast<const mono_asset_extension::MeshAsset*>(&mesh_asset);
    assert(mesh_asset_ptr != nullptr && "Failed to get box mesh asset!");

    // Fill setup param from mesh asset
    param->mesh_set_.vertex_buffer_handles = *mesh_asset_ptr->GetVertexBufferHandles();
    param->mesh_set_.index_buffer_handles = *mesh_asset_ptr->GetIndexBufferHandles();
    param->mesh_set_.index_counts = *mesh_asset_ptr->GetIndexCounts();

    std::vector<const render_graph::MaterialHandle*> material_handles;
    material_handles.resize(mesh_asset_ptr->GetIndexCounts()->size());
    for (int i = 0; i < material_handles.size(); ++i)
        material_handles[i] = render_graph::MaterialHandleManager::GetInstance().GetMaterialHandle(
            BoxLambertMaterialHandleKey().Value());
    param->mesh_set_.material_handles = std::move(material_handles);

    // Store mesh asset handle
    param->mesh_set_.mesh_asset_handle = BoxMeshAssetHandle::ID();

    return param; // Return the setup param
}

} // namespace mono_forge_app_template