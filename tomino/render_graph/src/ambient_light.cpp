#include "render_graph/src/pch.h"
#include "render_graph/include/ambient_light.h"

namespace render_graph
{

bool AmbientLight::Setup(Light::SetupParam& param)
{
    // Cast to AmbientLight::SetupParam
    SetupParam* ambient_light_param = dynamic_cast<SetupParam*>(&param);
    assert(ambient_light_param != nullptr && "Invalid SetupParam type for AmbientLight.");

    // Store light buffer data
    buffer_.light_type = AmbientLightTypeHandle::ID();
    buffer_.color = ambient_light_param->color;
    buffer_.intensity = ambient_light_param->intensity;

    return true; // Setup successful
}

bool AmbientLight::Apply(const Light::SetupParam& param)
{
    // Cast to AmbientLight::SetupParam
    const SetupParam* ambient_light_param = dynamic_cast<const SetupParam*>(&param);
    assert(ambient_light_param != nullptr && "Invalid SetupParam type for AmbientLight.");

    // Update light buffer data
    buffer_.color = ambient_light_param->color;
    buffer_.intensity = ambient_light_param->intensity;

    return true; // Apply successful
}

LightTypeHandleID AmbientLight::GetLightTypeHandleID() const
{
    return AmbientLightTypeHandle::ID();
}

void AmbientLight::UpdateViewProjMatrix()
{
    // Ambient light does not have a view-projection matrix, so this is a no-op.
}

} // namespace render_graph