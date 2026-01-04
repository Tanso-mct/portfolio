#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"
#include "mono_asset_extension/include/dll_config.h"

namespace mono_asset_extension
{

// The handle class for the system
class MONO_ASSET_EXT_DLL AssetRequestProcessorSystemHandle :
    public ecs::SystemHandle<AssetRequestProcessorSystemHandle> {};

// The system class
class MONO_ASSET_EXT_DLL AssetRequestProcessorSystem :
    public ecs::System
{
public:
    AssetRequestProcessorSystem(std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy);
    virtual ~AssetRequestProcessorSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    std::unique_ptr<mono_service::ServiceProxy> asset_service_proxy = nullptr;
};


} // namespace mono_asset_extension