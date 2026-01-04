#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL EntityEditorSystemHandle :
    public ecs::SystemHandle<EntityEditorSystemHandle> {};

// The system class
// This system manages entity editing functionalities
class MONO_ENTITY_ARCHIVE_EXT_DLL EntityEditorSystem :
    public ecs::System
{
public:
    EntityEditorSystem(
        std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy,
        mono_service::ServiceProxyManager& service_proxy_manager);
    virtual ~EntityEditorSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The entity archive service proxy
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_ = nullptr;

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;
};


} // namespace mono_entity_archive_extension