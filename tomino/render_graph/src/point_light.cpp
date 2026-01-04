#include "render_graph/src/pch.h"
#include "render_graph/include/point_light.h"

namespace render_graph
{

bool PointLight::Setup(Light::SetupParam& param)
{
    // Cast to PointLight::SetupParam
    SetupParam* point_light_param = dynamic_cast<SetupParam*>(&param);
    assert(point_light_param != nullptr && "Invalid SetupParam type for PointLight.");

    // Store light buffer data
    buffer_.light_type = PointLightTypeHandle::ID();
    buffer_.color = point_light_param->color;
    buffer_.intensity = point_light_param->intensity;
    buffer_.range = point_light_param->range;

    return true; // Setup successful
}

bool PointLight::Apply(const Light::SetupParam& param)
{
    // Cast to PointLight::SetupParam
    const SetupParam* point_light_param = dynamic_cast<const SetupParam*>(&param);
    assert(point_light_param != nullptr && "Invalid SetupParam type for PointLight.");

    // Update light buffer data
    buffer_.color = point_light_param->color;
    buffer_.intensity = point_light_param->intensity;
    buffer_.range = point_light_param->range;

    return true; // Apply successful
}

LightTypeHandleID PointLight::GetLightTypeHandleID() const
{
    return PointLightTypeHandle::ID();
}

void PointLight::UpdateViewProjMatrix()
{
    // TODO: Implement view-projection matrix update for point light
}

} // namespace render_graph