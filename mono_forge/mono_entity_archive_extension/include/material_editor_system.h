#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"

#include "mono_entity_archive_extension/include/dll_config.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_proxy_manager.h"

namespace mono_entity_archive_extension
{

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL MaterialEditorSystemHandle :
    public ecs::SystemHandle<MaterialEditorSystemHandle> {};

// The system class
class MONO_ENTITY_ARCHIVE_EXT_DLL MaterialEditorSystem :
    public ecs::System
{
public:
    MaterialEditorSystem(
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy,
        mono_service::ServiceProxyManager& service_proxy_manager);
    virtual ~MaterialEditorSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_ = nullptr;

    // The entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;
};


} // namespace mono_entity_archive_extension