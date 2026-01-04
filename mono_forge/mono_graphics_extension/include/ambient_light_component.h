#pragma once

#include <DirectXMath.h>

#include "ecs/include/component.h"
#include "render_graph/include/light_handle.h"
#include "mono_service/include/service.h"

#include "mono_graphics_extension/include/dll_config.h"
#include "mono_graphics_extension/include/light_component.h"

namespace mono_graphics_extension
{

namespace ambient_light
{

constexpr DirectX::XMFLOAT4 DEFAULT_LIGHT_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr float DEFAULT_INTENSITY = 0.2f;

} // namespace ambient_light

// The handle class for the component
class MONO_GRAPHICS_EXT_DLL AmbientLightComponentHandle : public ecs::ComponentHandle<AmbientLightComponentHandle> {};

// The component class
class MONO_GRAPHICS_EXT_DLL AmbientLightComponent : //REFLECTABLE_COMMENT_BEGIN//
    public LightComponent
{
public:
    AmbientLightComponent(
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy, uint32_t back_buffer_count);
    virtual ~AmbientLightComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            light_color(ambient_light::DEFAULT_LIGHT_COLOR),
            intensity(ambient_light::DEFAULT_INTENSITY)
        {
        }

        virtual ~SetupParam() override = default;

        // The light color
        DirectX::XMFLOAT4 light_color; //REFLECTED//

        // The light intensity
        float intensity; //REFLECTED//

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;
    virtual std::unique_ptr<render_graph::Light::SetupParam> GetLightSetupParam() const override;
    virtual const render_graph::LightHandle* GetLightHandle() const override { return &light_handle_; }
    virtual bool CastShadow() const override { return false; }

    // Get light color
    const DirectX::XMFLOAT4& GetLightColor() const { return light_color_; }

    // Set light color
    void SetLightColor(const DirectX::XMFLOAT4& color) { light_color_ = color; }

    // Get intensity
    float GetIntensity() const { return intensity_; }

    // Set intensity
    void SetIntensity(float intensity) { intensity_ = intensity; }

private:
    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // The back buffer count
    const uint32_t back_buffer_count_;

    // The light color
    DirectX::XMFLOAT4 light_color_ = ambient_light::DEFAULT_LIGHT_COLOR;

    // The light intensity
    float intensity_ = ambient_light::DEFAULT_INTENSITY;

    // The light handle
    render_graph::LightHandle light_handle_ = render_graph::LightHandle();

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_graphics_extension