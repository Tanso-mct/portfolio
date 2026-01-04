#pragma once

#include <DirectXMath.h>

#include "ecs/include/component.h"
#include "render_graph/include/light_handle.h"
#include "mono_service/include/service.h"

#include "mono_graphics_extension/include/dll_config.h"
#include "mono_graphics_extension/include/light_component.h"

namespace mono_graphics_extension
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

} // namespace directional_light

// The handle class for the component
class MONO_GRAPHICS_EXT_DLL DirectionalLightComponentHandle : public ecs::ComponentHandle<DirectionalLightComponentHandle> {};

// The component class
class MONO_GRAPHICS_EXT_DLL DirectionalLightComponent : //REFLECTABLE_COMMENT_BEGIN//
    public LightComponent
{
public:
    DirectionalLightComponent(
        std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy, uint32_t back_buffer_count);
    virtual ~DirectionalLightComponent() override;

    class SetupParam : //REFLECTABLE_SETUP_PARAM_BEGIN//
        public ecs::Component::SetupParam
    {
    public:
        SetupParam() :
            light_color(directional_light::DEFAULT_LIGHT_COLOR),
            intensity(directional_light::DEFAULT_INTENSITY),
            distance(directional_light::DEFAULT_DISTANCE),
            ortho_width(directional_light::DEFAULT_ORTHO_WIDTH),
            ortho_height(directional_light::DEFAULT_ORTHO_HEIGHT),
            near_z(directional_light::DEFAULT_NEAR_Z),
            far_z(directional_light::DEFAULT_FAR_Z),
            shadow_map_size(directional_light::DEFAULT_SHADOW_MAP_SIZE)
        {
        }
        virtual ~SetupParam() override = default;

        // The light color
        DirectX::XMFLOAT4 light_color; //REFLECTED//

        // The light intensity
        float intensity; //REFLECTED//

        // The light distance
        float distance; //REFLECTED//

        // The orthographic projection width
        float ortho_width; //REFLECTED//

        // The orthographic projection height
        float ortho_height; //REFLECTED//

        // The near clipping plane distance
        float near_z; //REFLECTED//

        // The far clipping plane distance
        float far_z; //REFLECTED//

        // The shadow map size (width and height)
        uint32_t shadow_map_size; //REFLECTED//

    }; //REFLECTABLE_SETUP_PARAM_END//

    virtual bool Setup(ecs::Component::SetupParam& param) override;
    virtual bool Apply(const ecs::Component::SetupParam& param) override;
    virtual ecs::ComponentID GetID() const override;
    virtual std::unique_ptr<render_graph::Light::SetupParam> GetLightSetupParam() const override;
    virtual const render_graph::LightHandle* GetLightHandle() const override { return &light_handle_; }
    virtual bool CastShadow() const override { return true; }

    // Get light color
    const DirectX::XMFLOAT4& GetLightColor() const { return light_color_; }

    // Set light color
    void SetLightColor(const DirectX::XMFLOAT4& color) { light_color_ = color; }

    // Get intensity
    float GetIntensity() const { return intensity_; }

    // Set intensity
    void SetIntensity(float intensity) { intensity_ = intensity; }

    // Get distance
    float GetDistance() const { return distance_; }

    // Set distance
    void SetDistance(float distance) { distance_ = distance; }

    // Get orthographic projection width
    float GetOrthoWidth() const { return ortho_width_; }

    // Set orthographic projection width
    void SetOrthoWidth(float ortho_width) { ortho_width_ = ortho_width; }

    // Get orthographic projection height
    float GetOrthoHeight() const { return ortho_height_; }

    // Set orthographic projection height
    void SetOrthoHeight(float ortho_height) { ortho_height_ = ortho_height; }

    // Get near clipping plane distance
    float GetNearZ() const { return near_z_; }

    // Set near clipping plane distance
    void SetNearZ(float near_z) { near_z_ = near_z; }

    // Get far clipping plane distance
    float GetFarZ() const { return far_z_; }

    // Set far clipping plane distance
    void SetFarZ(float far_z) { far_z_ = far_z; }

private:
    // The graphics service proxy
    std::unique_ptr<mono_service::ServiceProxy> graphics_service_proxy_ = nullptr;

    // The back buffer count
    const uint32_t back_buffer_count_;

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

}; //REFLECTABLE_COMMENT_END//

} // namespace mono_graphics_extension