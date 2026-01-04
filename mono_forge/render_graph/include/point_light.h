#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/light.h"

namespace render_graph
{

// Light type handle for PointLight light
class RENDER_GRAPH_DLL PointLightTypeHandle : public LightTypeHandle<PointLightTypeHandle> {};

// Directional light class
class RENDER_GRAPH_DLL PointLight : 
    public Light
{
public:
    PointLight() = default;
    ~PointLight() override = default;

    class SetupParam : 
        public Light::SetupParam
    {
    public:
        SetupParam() :
            color(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
            intensity(1.0f),
            range(0.0f)
        {
        }

        ~SetupParam() override = default;

        // Color of the point light
        DirectX::XMFLOAT4 color;

        // Intensity of the point light
        float intensity;

        // Range of the point light
        float range;
    };

    bool Setup(Light::SetupParam& param) override;
    bool Apply(const Light::SetupParam& param) override;
    LightTypeHandleID GetLightTypeHandleID() const override;
    void UpdateViewProjMatrix() override;

    // Get color of the point light
    const DirectX::XMFLOAT4& GetColor() const { return buffer_.color; }

    // Set color of the point light
    void SetColor(const DirectX::XMFLOAT4& color) { buffer_.color = color; }

    // Get intensity of the point light
    float GetIntensity() const { return buffer_.intensity; }

    // Set intensity of the point light
    void SetIntensity(float intensity) { buffer_.intensity = intensity; }

    // Get range of the point light
    float GetRange() const { return buffer_.range; }

    // Set range of the point light
    void SetRange(float range) { buffer_.range = range; }
};

} // namespace render_graph