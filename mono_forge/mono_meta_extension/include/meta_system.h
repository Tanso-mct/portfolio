#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"

#include "mono_meta_extension/include/dll_config.h"
#include "mono_meta_extension/include/meta_tag_generator.h"
#include "mono_meta_extension/include/meta_layer_generator.h"

namespace mono_meta_extension
{

// The handle class for the system
class MONO_META_EXT_DLL MetaSystemHandle :
    public ecs::SystemHandle<MetaSystemHandle> {};

// The system class
class MONO_META_EXT_DLL MetaSystem :
    public ecs::System
{
public:
    MetaSystem();
    virtual ~MetaSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // Singleton tag generator
    std::unique_ptr<MetaTagGenerator> tag_generator_ = nullptr;

    // Singleton layer generator
    std::unique_ptr<MetaLayerGenerator> layer_generator_ = nullptr;
};


} // namespace mono_meta_extension