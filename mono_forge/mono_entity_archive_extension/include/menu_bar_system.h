#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"

#include "mono_entity_archive_extension/include/dll_config.h"

namespace mono_entity_archive_extension
{

// The handle class for the system
class MONO_ENTITY_ARCHIVE_EXT_DLL MenuBarSystemHandle :
    public ecs::SystemHandle<MenuBarSystemHandle> {};

// The system class
class MONO_ENTITY_ARCHIVE_EXT_DLL MenuBarSystem :
    public ecs::System
{
public:
    MenuBarSystem();
    virtual ~MenuBarSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:

};


} // namespace mono_entity_archive_extension