#pragma once
#include "mono_transform/include/dll_config.h"

#include <DirectXMath.h>

namespace mono_transform
{
    MONO_TRANSFORM_API void XMQuaternionToEulerAngles(const DirectX::XMVECTOR& q, float& pitch, float& yaw, float& roll);

} // namespace mono_transform