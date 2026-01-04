#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "render_graph/include/light_handle.h"
#include "mono_service/include/service.h"
#include "mono_render/include/light_component.h"

namespace mono_render
{

namespace ambient_light
{

    constexpr DirectX::XMFLOAT4 DEFAULT_LIGHT_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float DEFAULT_INTENSITY = 0.2f;
    constexpr uint32_t DEFAULT_BACK_BUFFER_COUNT = 2;

} // namespace ambient_light

    constexpr size_t AmbientLightComponentMaxCount = 10;
    class MONO_RENDER_API AmbientLightComponent :
        public LightComponent
    {
    private:
        // The graphics service proxy
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

        // The back buffer count
        uint32_t back_buffer_count_ = ambient_light::DEFAULT_BACK_BUFFER_COUNT;

        // The light color
        DirectX::XMFLOAT4 light_color_ = ambient_light::DEFAULT_LIGHT_COLOR;

        // The light intensity
        float intensity_ = ambient_light::DEFAULT_INTENSITY;

        // The light handle
        render_graph::LightHandle light_handle_ = render_graph::LightHandle();

    public:
        AmbientLightComponent();
        ~AmbientLightComponent();

        struct SetupParam
        {
            // The light color
            DirectX::XMFLOAT4 light_color = ambient_light::DEFAULT_LIGHT_COLOR;

            // The light intensity
            float intensity = ambient_light::DEFAULT_INTENSITY;
        };
        void Setup(SetupParam &param);
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

    };

    extern MONO_RENDER_API riaecs::ComponentRegistrar
    <AmbientLightComponent, AmbientLightComponentMaxCount> AmbientLightComponentID;

} // namespace mono_render