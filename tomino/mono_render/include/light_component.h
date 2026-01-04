#pragma once

#include "render_graph/include/light_handle.h"
#include "render_graph/include/light.h"

namespace mono_render
{

class LightComponent
{
public:
    virtual ~LightComponent() = default;

    // Get light setup parameter
    virtual std::unique_ptr<render_graph::Light::SetupParam> GetLightSetupParam() const = 0;

    // Get light handle
    virtual const render_graph::LightHandle* GetLightHandle() const = 0;

    // Check if the light casts shadows
    virtual bool CastShadow() const = 0;
};

} // namespace mono_graphics_extension