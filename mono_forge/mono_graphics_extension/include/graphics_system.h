#pragma once

#include "ecs/include/system.h"
#include "mono_service/include/service.h"
#include "render_graph/include/command_handle.h"
#include "mono_graphics_extension/include/dll_config.h"

namespace mono_graphics_extension
{

// The number of back buffers
constexpr uint32_t BACK_BUFFER_COUNT = 2;

// Common direction vectors
constexpr DirectX::XMFLOAT4 FORWARD_VECTOR = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
constexpr DirectX::XMFLOAT4 UP_VECTOR = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

// The handle class for the graphics system
class MONO_GRAPHICS_EXT_DLL GraphicsSystemHandle :
    public ecs::SystemHandle<GraphicsSystemHandle> {};

// The graphics system class
class MONO_GRAPHICS_EXT_DLL GraphicsSystem :
    public ecs::System
{
public:
    GraphicsSystem(std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy);
    virtual ~GraphicsSystem() override;

    virtual bool PreUpdate(ecs::World& world) override;
    virtual bool Update(ecs::World& world) override;
    virtual bool PostUpdate(ecs::World& world) override;
    virtual ecs::SystemID GetID() const override;

private:
    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;
};


} // namespace mono_graphics_extension