#pragma once
#include "mono_render/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "render_graph/include/light_handle.h"
#include "mono_service/include/service.h"
#include "mono_render/include/light_component.h"

namespace mono_render
{

namespace directional_light
{

    constexpr DirectX::XMFLOAT4 DEFAULT_LIGHT_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    constexpr float DEFAULT_INTENSITY = 1.0f;
    constexpr float DEFAULT_DISTANCE = 10.0f;
    constexpr float DEFAULT_ORTHO_WIDTH = 10.0f;
    constexpr float DEFAULT_ORTHO_HEIGHT = 10.0f;
    constexpr float DEFAULT_NEAR_Z = 0.1f;
    constexpr float DEFAULT_FAR_Z = 100.0f;
    constexpr uint32_t DEFAULT_SHADOW_MAP_SIZE = 2048;
    constexpr uint32_t DEFAULT_BACK_BUFFER_COUNT = 2;

} // namespace directional_light

    constexpr size_t DirectionalLightComponentMaxCount = 5;
    class MONO_RENDER_API DirectionalLightComponent :
        public LightComponent
    {
    private:
        // The graphics service proxy
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

        // The back buffer count
        uint32_t back_buffer_count_ = directional_light::DEFAULT_BACK_BUFFER_COUNT;

        // The light color
        DirectX::XMFLOAT4 light_color_ = directional_light::DEFAULT_LIGHT_COLOR;

        // The light intensity
        float intensity_ = directional_light::DEFAULT_INTENSITY;

        // The light distance
        float distance_ = directional_light::DEFAULT_DISTANCE;

        // The orthographic projection width
        float ortho_width_ = directional_light::DEFAULT_ORTHO_WIDTH;

        // The orthographic projection height
        float ortho_height_ = directional_light::DEFAULT_ORTHO_HEIGHT;

        // The near clipping plane distance
        float near_z_ = directional_light::DEFAULT_NEAR_Z;

        // The far clipping plane distance
        float far_z_ = directional_light::DEFAULT_FAR_Z;

        // The shadow map size (width and height)
        uint32_t shadow_map_size_ = directional_light::DEFAULT_SHADOW_MAP_SIZE;

        // The light handle
        render_graph::LightHandle light_handle_ = render_graph::LightHandle();

    public:
        DirectionalLightComponent();
        ~DirectionalLightComponent();

        struct SetupParam
        {
            // The light color
            DirectX::XMFLOAT4 light_color = directional_light::DEFAULT_LIGHT_COLOR;

            // The light intensity
            float intensity = directional_light::DEFAULT_INTENSITY;

            // The light distance
            float distance = directional_light::DEFAULT_DISTANCE;

            // The orthographic projection width
            float ortho_width = directional_light::DEFAULT_ORTHO_WIDTH;

            // The orthographic projection height
            float ortho_height = directional_light::DEFAULT_ORTHO_HEIGHT;

            // The near clipping plane distance
            float near_z = directional_light::DEFAULT_NEAR_Z;

            // The far clipping plane distance
            float far_z = directional_light::DEFAULT_FAR_Z;

            // The shadow map size (width and height)
            uint32_t shadow_map_size = directional_light::DEFAULT_SHADOW_MAP_SIZE;
        };
        void Setup(SetupParam &param);
        virtual std::unique_ptr<render_graph::Light::SetupParam> GetLightSetupParam() const override;
        virtual const render_graph::LightHandle* GetLightHandle() const override { return &light_handle_; }
        virtual bool CastShadow() const override { return true; }

    };

    extern MONO_RENDER_API riaecs::ComponentRegistrar
    <DirectionalLightComponent, DirectionalLightComponentMaxCount> DirectionalLightComponentID;

} // namespace mono_render