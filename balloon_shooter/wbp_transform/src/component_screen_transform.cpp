#include "wbp_transform/src/pch.h"
#include "wbp_transform/include/component_screen_transform.h"

using namespace DirectX;

const WBP_TRANSFORM_API size_t &wbp_transform::ScreenTransformComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

namespace
{
    void XMQuaternionToEulerAngles(FXMVECTOR q, float* pitch, float* yaw, float* roll)
    {
        XMFLOAT4 qf;
        XMStoreFloat4(&qf, q);

        // Calculate the angular radians of each axis
        float ysqr = qf.y * qf.y;

        // roll (X)
        float t0 = +2.0f * (qf.w * qf.x + qf.y * qf.z);
        float t1 = +1.0f - 2.0f * (qf.x * qf.x + ysqr);
        *roll = std::atan2(t0, t1);

        // pitch (Y)
        float t2 = +2.0f * (qf.w * qf.y - qf.z * qf.x);
        t2 = t2 > 1.0f ? 1.0f : t2;
        t2 = t2 < -1.0f ? -1.0f : t2;
        *pitch = std::asin(t2);

        // yaw (Z)
        float t3 = +2.0f * (qf.w * qf.z + qf.x * qf.y);
        float t4 = +1.0f - 2.0f * (ysqr + qf.z * qf.z);
        *yaw = std::atan2(t3, t4);
    }

} // namespace

wbp_transform::ScreenTransformComponent::ScreenTransformComponent()
{
    localPosition_ = XMFLOAT3(0.0f, 0.0f, 0.0f);
    quatLocalRotation_ = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    localScale_ = ORIGINAL_SCALE;
    baseSize_ = DEFAULT_BASE_SIZE;
}

const size_t &wbp_transform::ScreenTransformComponent::GetID() const
{
    return ScreenTransformComponentID();
}

void wbp_transform::ScreenTransformComponent::SetLocalRotation(const DirectX::XMFLOAT3 &localRot)
{
    float pitch = XMConvertToRadians(localRot.x);
    float yaw   = XMConvertToRadians(localRot.y);
    float roll  = XMConvertToRadians(localRot.z);
    XMVECTOR q = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
    XMStoreFloat4(&quatLocalRotation_, q);
}

void wbp_transform::ScreenTransformComponent::Translate(const DirectX::XMFLOAT3 &translation)
{
    localPosition_.x += translation.x;
    localPosition_.y += translation.y;
    localPosition_.z += translation.z;
}

void wbp_transform::ScreenTransformComponent::Rotate(const DirectX::XMFLOAT3 &rotation)
{
    float pitch = XMConvertToRadians(rotation.x);
    float yaw   = XMConvertToRadians(rotation.y);
    float roll  = XMConvertToRadians(rotation.z);

    DirectX::XMVECTOR q = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);

    XMVECTOR newQuat = XMQuaternionMultiply(q, XMLoadFloat4(&quatLocalRotation_));
    XMStoreFloat4(&quatLocalRotation_, newQuat);
}

void wbp_transform::ScreenTransformComponent::Scale(const DirectX::XMFLOAT3 &scale)
{
    localScale_.x *= scale.x;
    localScale_.y *= scale.y;
    localScale_.z *= scale.z;
}

void wbp_transform::ScreenTransformComponent::TranslateTopLine(float translation)
{
    // Update the local scale
    localScale_ = XMFLOAT3
    (
        localScale_.x,
        localScale_.y + translation / (ORIGINAL_SCALE.y * 2.0f),
        localScale_.z
    );

    // Move the center (position.y) by translation/2
    localPosition_.y += translation / 2.0f;
}

void wbp_transform::ScreenTransformComponent::TranslateBottomLine(float translation)
{
    float invertedTranslation = -translation; // Invert the translation for bottom line

    // Update the local scale
    localScale_ = XMFLOAT3
    (
        localScale_.x,
        localScale_.y + invertedTranslation / (ORIGINAL_SCALE.y * 2.0f),
        localScale_.z
    );

    // Move the center (position.y) by -invertedTranslation/2
    localPosition_.y -= invertedTranslation / 2.0f;
}

void wbp_transform::ScreenTransformComponent::TranslateLeftLine(float translation)
{
    float invertedTranslation = -translation; // Invert the translation for bottom line

    // Update the local scale
    localScale_ = XMFLOAT3
    (
        localScale_.x + invertedTranslation / (ORIGINAL_SCALE.x * 2.0f),
        localScale_.y,
        localScale_.z
    );

    // Move the center (position.x) by -invertedTranslation/2
    localPosition_.x -= invertedTranslation / 2.0f; 
}

void wbp_transform::ScreenTransformComponent::TranslateRightLine(float translation)
{
    // Update the local scale
    localScale_ = XMFLOAT3
    (
        localScale_.x + translation / (ORIGINAL_SCALE.x * 2.0f),
        localScale_.y,
        localScale_.z
    );

    // Move the center (position.x) by translation/2
    localPosition_.x += translation / 2.0f;
}

DirectX::XMMATRIX wbp_transform::ScreenTransformComponent::GetClipMatrix() const
{
    XMFLOAT3 clipPosition = XMFLOAT3
    (
        localPosition_.x / (baseSize_.x / 2.0f),
        localPosition_.y / (baseSize_.y / 2.0f),
        localPosition_.z / (baseSize_.z / 2.0f)
    );

    XMFLOAT3 clipScale = XMFLOAT3
    (
        localScale_.x / (baseSize_.x / 2.0f),
        localScale_.y / (baseSize_.y / 2.0f),
        localScale_.z / (baseSize_.z / 2.0f)
    );

    XMMATRIX scaleMat = XMMatrixScaling(clipScale.x, clipScale.y, clipScale.z);
    XMMATRIX rotMat = XMMatrixRotationQuaternion(XMLoadFloat4(&quatLocalRotation_));
    XMMATRIX transMat = XMMatrixTranslation(clipPosition.x, clipPosition.y, clipPosition.z);
    
    return scaleMat * rotMat * transMat;
}

namespace wbp_transform
{
    WB_REGISTER_COMPONENT(ScreenTransformComponentID(), ScreenTransformComponent);

} // namespace wbp_transform