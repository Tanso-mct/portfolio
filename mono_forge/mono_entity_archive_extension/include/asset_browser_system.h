#pragma once

#include "ecs/include/entity.h"
#include "ecs/include/system.h"
#include "mono_service/include/service.h"
#include "mono_service/include/service_proxy_manager.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL AssetBrowserSystemHandle :
    public ecs::SystemHandle<AssetBrowserSystemHandle> {};

// The system class
class MONO_ENTITY_ARCHIVE_EXT_DLL AssetBrowserSystem :
    public ecs::System
{
public:
    AssetBrowserSystem(mono_service::ServiceProxyManager& service_proxy_manager);
    virtual ~AssetBrowserSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The entity for asset request component
    ecs::Entity asset_request_entity_ = ecs::Entity();

    // The service proxy manager reference
    mono_service::ServiceProxyManager& service_proxy_manager_;



};


} // namespace mono_entity_archive_extension