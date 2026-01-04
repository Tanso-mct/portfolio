#include "wbp_render/src/pch.h"
#include "wbp_render/include/component_camera.h"

const WBP_RENDER_API size_t &wbp_render::CameraComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &wbp_render::CameraComponent::GetID() const
{
    return CameraComponentID();
}

namespace wbp_render
{
    WB_REGISTER_COMPONENT(CameraComponentID(), CameraComponent);

} // namespace wbp_render