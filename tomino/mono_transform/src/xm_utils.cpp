#include "mono_transform/src/pch.h"
#include "mono_transform/include/xm_utils.h"

using namespace DirectX;

MONO_TRANSFORM_API void mono_transform::XMQuaternionToEulerAngles(const XMVECTOR& q, float& pitch, float& yaw, float& roll)
{
    XMMATRIX m = XMMatrixRotationQuaternion(q);

    // Decomposition of Euler angles (right-hand system, Yaw=Y, Pitch=X, Roll=Z, YXZ order)

    // Pitch
    pitch = asinf(-m.r[2].m128_f32[1]); // -m32

    // Yaw, Roll
    if (cosf(pitch) > 1e-6) 
    {
        yaw  = atan2f(m.r[2].m128_f32[0], m.r[2].m128_f32[2]); // m31, m33
        roll = atan2f(m.r[0].m128_f32[1], m.r[1].m128_f32[1]); // m12, m22
    } 
    else 
    {
        // When gimbal locked
        yaw  = atan2f(-m.r[0].m128_f32[2], m.r[0].m128_f32[0]);
        roll = 0.0f;
    }
}