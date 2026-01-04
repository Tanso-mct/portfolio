#pragma once

#include <DirectXMath.h>

#include "mono_service/include/service_view.h"

#include "mono_transform_service/include/dll_config.h"
#include "mono_transform_service/include/transform_service.h"

namespace mono_transform_service
{

class MONO_TRANSFORM_SERVICE_DLL TransformServiceView :
    public mono_service::ServiceView
{
public:
    TransformServiceView(const mono_service::ServiceAPI& service_api);
    virtual ~TransformServiceView() override = default;

    // Get the world matrix of a Transform using its Handle
    DirectX::XMMATRIX GetWorldMatrix(const transform_evaluator::TransformHandle& handle) const;

    // Get the translation of a Transform using its Handle
    DirectX::XMFLOAT3 GetTranslation(const transform_evaluator::TransformHandle& handle) const;

    // Get the rotation of a Transform using its Handle
    DirectX::XMFLOAT4 GetRotation(const transform_evaluator::TransformHandle& handle) const;

    // Get the scale of a Transform using its Handle
    DirectX::XMFLOAT3 GetScale(const transform_evaluator::TransformHandle& handle) const;
};

} // namespace mono_transform_service