#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "render_graph/include/light_handle.h"
#include "mono_service/include/service.h"
#include "mono_render/include/light_component.h"

namespace mono_render
{

namespace point_light
{

    constexpr DirectX::XMFLOAT4 DEFAULT_LIGHT_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float DEFAULT_INTENSITY = 1.0f;
    constexpr float DEFAULT_RANGE = 5.0f;
    constexpr uint32_t DEFAULT_BACK_BUFFER_COUNT = 2;

} // namespace point_light

    constexpr size_t PointLightComponentMaxCount = 100;
    class MONO_RENDER_API PointLightComponent :
        public LightComponent
    {
    private:
        // The graphics service proxy
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

        // The back buffer count
        uint32_t back_buffer_count_ = point_light::DEFAULT_BACK_BUFFER_COUNT;

        // The light handle
        render_graph::LightHandle light_handle_ = render_graph::LightHandle();

        // The light color
        DirectX::XMFLOAT4 light_color_ = point_light::DEFAULT_LIGHT_COLOR;

        // The light intensity
        float intensity_ = point_light::DEFAULT_INTENSITY;

        // The light range
        float range_ = point_light::DEFAULT_RANGE;

    public:
        PointLightComponent();
        ~PointLightComponent();

        struct SetupParam
        {
            // The light color
            DirectX::XMFLOAT4 light_color = point_light::DEFAULT_LIGHT_COLOR;

            // The light intensity
            float intensity = point_light::DEFAULT_INTENSITY;

            // The light range
            float range = point_light::DEFAULT_RANGE;
        };
        void Setup(SetupParam &param);
        virtual std::unique_ptr<render_graph::Light::SetupParam> GetLightSetupParam() const override;
        virtual const render_graph::LightHandle* GetLightHandle() const override { return &light_handle_; }
        virtual bool CastShadow() const override { return false; }

    };

    extern MONO_RENDER_API riaecs::ComponentRegistrar
    <PointLightComponent, PointLightComponentMaxCount> PointLightComponentID;

} // namespace mono_render