#pragma once

#include "component_editor/include/component_adder.h"

namespace mono_forge_app_template
{

// Component adder for MetaComponent
class MetaComponentAdder :
    public component_editor::ComponentAdder
{
public:
    MetaComponentAdder() = default;
    virtual ~MetaComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for TransformComponent
class TransformComponentAdder :
    public component_editor::ComponentAdder
{
public:
    TransformComponentAdder() = default;
    virtual ~TransformComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for CameraComponent
class CameraComponentAdder :
    public component_editor::ComponentAdder
{
public:
    CameraComponentAdder() = default;
    virtual ~CameraComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for DirectionalLightComponent
class DirectionalLightComponentAdder :
    public component_editor::ComponentAdder
{
public:
    DirectionalLightComponentAdder() = default;
    virtual ~DirectionalLightComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for AmbientLightComponent
class AmbientLightComponentAdder :
    public component_editor::ComponentAdder
{
public:
    AmbientLightComponentAdder() = default;
    virtual ~AmbientLightComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for PointLightComponent
class PointLightComponentAdder :
    public component_editor::ComponentAdder
{
public:
    PointLightComponentAdder() = default;
    virtual ~PointLightComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

// Component adder for RenderableComponent
class RenderableComponentAdder :
    public component_editor::ComponentAdder
{
public:
    RenderableComponentAdder() = default;
    virtual ~RenderableComponentAdder() = default;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual bool Add(
        ecs::World& world, const ecs::Entity& entity,
        std::unique_ptr<ecs::Component::SetupParam> setup_param,
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
    virtual std::unique_ptr<ecs::Component::SetupParam> GetSetupParam(
        mono_service::ServiceProxyManager& service_proxy_manager) const override;
};

} // namespace mono_forge_app_template