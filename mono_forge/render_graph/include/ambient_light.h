#pragma once

#include "render_graph/include/dll_config.h"
#include "render_graph/include/light.h"

namespace render_graph
{

// Light type handle for Ambient light
class RENDER_GRAPH_DLL AmbientLightTypeHandle : public LightTypeHandle<AmbientLightTypeHandle> {};

// Directional light class
class RENDER_GRAPH_DLL AmbientLight : 
    public Light
{
public:
    AmbientLight() = default;
    ~AmbientLight() override = default;

    class SetupParam : 
        public Light::SetupParam
    {
    public:
        SetupParam() :
            color(DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)),
            intensity(1.0f)
        {
        }

        ~SetupParam() override = default;

        // Color of the ambient light
        DirectX::XMFLOAT4 color;

        // Intensity of the ambient light
        float intensity;
    };

    bool Setup(Light::SetupParam& param) override;
    bool Apply(const Light::SetupParam& param) override;
    LightTypeHandleID GetLightTypeHandleID() const override;
    void UpdateViewProjMatrix() override;

    // Get color of the ambient light
    const DirectX::XMFLOAT4& GetColor() const { return buffer_.color; }

    // Set color of the ambient light
    void SetColor(const DirectX::XMFLOAT4& color) { buffer_.color = color; }

    // Get intensity of the ambient light
    float GetIntensity() const { return buffer_.intensity; }

    // Set intensity of the ambient light
    void SetIntensity(float intensity) { buffer_.intensity = intensity; }
};

} // namespace render_graph