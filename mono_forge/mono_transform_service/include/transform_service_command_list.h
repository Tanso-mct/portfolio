#pragma once

#include <DirectXMath.h>

#include "mono_service/include/service_command.h"

#include "mono_transform_service/include/dll_config.h"
#include "mono_transform_service/include/transform_service.h"

namespace mono_transform_service
{

// The graphics command list class
class MONO_TRANSFORM_SERVICE_DLL TransformServiceCommandList :
    public mono_service::ServiceCommandList
{
public:
    TransformServiceCommandList() = default;
    ~TransformServiceCommandList() override = default;

    // Create a Transform and output its Handle
    void CreateTransform(
        transform_evaluator::TransformHandle& out_handle, 
        DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
        DirectX::XMFLOAT4 rotation = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f),
        DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

    // Update the world TRS of a Transform using its Handle
    void UpdateWorldTRS(
        const transform_evaluator::TransformHandle& handle,
        const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT4& rotation, const DirectX::XMFLOAT3& scale);

    // Update the world translation of a Transform using its Handle
    void UpdateWorldTranslation(
        const transform_evaluator::TransformHandle& handle, const DirectX::XMFLOAT3& translation);

    // Update the world rotation of a Transform using its Handle
    void UpdateWorldRotation(
        const transform_evaluator::TransformHandle& handle, const DirectX::XMFLOAT4& rotation);

    // Update the world scale of a Transform using its Handle
    void UpdateWorldScale(
        const transform_evaluator::TransformHandle& handle, const DirectX::XMFLOAT3& scale);

    // Destroy a Transform using its Handle
    void DestroyTransform(const transform_evaluator::TransformHandle& handle);
        
};

} // namespace mono_transform_service