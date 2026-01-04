#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/light.h"

namespace render_graph
{

namespace directional_light
{

constexpr float DEFAULT_DISTANCE = 100.0f;
constexpr float DEFAULT_ORTHO_WIDTH = 100.0f;
constexpr float DEFAULT_ORTHO_HEIGHT = 100.0f;
constexpr float DEFAULT_NEAR_Z = 0.1f;
constexpr float DEFAULT_FAR_Z = 1000.0f;
constexpr DirectX::XMFLOAT4 DEFAULT_COLOR = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
constexpr float DEFAULT_INTENSITY = 1.0f;

} // namespace directional_light

// Light type handle for Directional light
class RENDER_GRAPH_DLL DirectionalLightTypeHandle : public LightTypeHandle<DirectionalLightTypeHandle> {};

// Directional light class
class RENDER_GRAPH_DLL DirectionalLight : 
    public Light
{
public:
    DirectionalLight() = default;
    ~DirectionalLight() override = default;

    class SetupParam : 
        public Light::SetupParam
    {
    public:
        SetupParam() :
            color(directional_light::DEFAULT_COLOR),
            intensity(directional_light::DEFAULT_INTENSITY)
        {
        }

        ~SetupParam() override = default;

        // Color of the directional light
        DirectX::XMFLOAT4 color;

        // Intensity of the directional light
        float intensity;

        // Distance from the light to the origin
        float distance = directional_light::DEFAULT_DISTANCE;

        // Orthographic width for the light's projection
        float ortho_width = directional_light::DEFAULT_ORTHO_WIDTH;

        // Orthographic height for the light's projection
        float ortho_height = directional_light::DEFAULT_ORTHO_HEIGHT;

        // Near plane distance for the light's projection
        float near_z = directional_light::DEFAULT_NEAR_Z;

        // Far plane distance for the light's projection
        float far_z = directional_light::DEFAULT_FAR_Z;

        // Shadow map size (width and height)
        uint32_t shadow_map_size = 1024;
    };

    bool Setup(Light::SetupParam& param) override;
    bool Apply(const Light::SetupParam& param) override;
    LightTypeHandleID GetLightTypeHandleID() const override;
    void UpdateViewProjMatrix();

    // Get color of the directional light
    const DirectX::XMFLOAT4& GetColor() const { return buffer_.color; }

    // Set color of the directional light
    void SetColor(const DirectX::XMFLOAT4& color) { buffer_.color = color; }

    // Get intensity of the directional light
    float GetIntensity() const { return buffer_.intensity; }

    // Set intensity of the directional light
    void SetIntensity(float intensity) { buffer_.intensity = intensity; }

    // Get distance from the light to the origin
    float GetDistance() const { return distance_; }

    // Set distance from the light to the origin
    void SetDistance(float distance) { distance_ = distance; }

    // Get orthographic width for the light's projection
    float GetOrthographicWidth() const { return ortho_width_; }

    // Set orthographic width for the light's projection
    void SetOrthographicWidth(float width) { ortho_width_ = width; }

    // Get orthographic height for the light's projection
    float GetOrthographicHeight() const { return ortho_height_; }

    // Set orthographic height for the light's projection
    void SetOrthographicHeight(float height) { ortho_height_ = height; }

    // Get near plane distance for the light's projection
    float GetNearZ() const { return near_z_; }

    // Set near plane distance for the light's projection
    void SetNearZ(float near_plane) { near_z_ = near_plane; }

    // Get far plane distance for the light's projection
    float GetFarZ() const { return far_z_; }

    // Set far plane distance for the light's projection
    void SetFarZ(float far_plane) { far_z_ = far_plane; }

private:
    // Distance from the light to the origin
    float distance_ = directional_light::DEFAULT_DISTANCE;

    // Orthographic projection width
    float ortho_width_ = directional_light::DEFAULT_ORTHO_WIDTH;

    // Orthographic projection height
    float ortho_height_ = directional_light::DEFAULT_ORTHO_HEIGHT;

    // Near plane distance for the light's projection
    float near_z_ = directional_light::DEFAULT_NEAR_Z;

    // Far plane distance for the light's projection
    float far_z_ = directional_light::DEFAULT_FAR_Z;

    // Shadow map size (width and height)
    uint32_t shadow_map_size_ = 1024;
};

} // namespace render_graph