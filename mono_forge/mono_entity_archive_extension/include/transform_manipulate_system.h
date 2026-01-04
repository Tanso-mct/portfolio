#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL TransformManipulateSystemHandle :
    public ecs::SystemHandle<TransformManipulateSystemHandle> {};

// The system class
class MONO_ENTITY_ARCHIVE_EXT_DLL TransformManipulateSystem :
    public ecs::System
{
public:
    TransformManipulateSystem(std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_);
    virtual ~TransformManipulateSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    std::unique_ptr<mono_service::ServiceProxy> entity_archive_service_proxy_;
};


} // namespace mono_entity_archive_extension