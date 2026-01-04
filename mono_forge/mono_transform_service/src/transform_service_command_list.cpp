#include "mono_transform_service/src/pch.h"
#include "mono_transform_service/include/transform_service_command_list.h"

using namespace DirectX;

#include "mono_transform_service/include/transform_service.h"

namespace mono_transform_service
{

void TransformServiceCommandList::CreateTransform(
    transform_evaluator::TransformHandle& out_handle, 
    XMFLOAT3 position, XMFLOAT4 rotation, XMFLOAT3 scale)
{
    AddCommand(
        [&out_handle, position, rotation, scale](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            // Create transform
            std::unique_ptr<transform_evaluator::Transform> transform
                = std::make_unique<transform_evaluator::Transform>(position, rotation, scale);

            // Add transform to transform adder
            out_handle = transform_service_api.GetTransformAdder().AddTransform(std::move(transform));
            if (!out_handle.IsValid())
                return false; // Failure

            return true; // Success
        });
}

void TransformServiceCommandList::UpdateWorldTRS(
    const transform_evaluator::TransformHandle& handle, 
    const DirectX::XMFLOAT3& translation, const DirectX::XMFLOAT4& rotation, const DirectX::XMFLOAT3& scale)
{
    AddCommand(
        [handle, translation, rotation, scale](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            transform_evaluator::TransformManager& transform_manager 
                = transform_evaluator::TransformManager::GetInstance();
            transform_manager.WithLock([&](transform_evaluator::TransformManager& manager)
            {
                // Get the Transform using its Handle
                transform_evaluator::Transform& transform = manager.GetTransform(handle);

                // Create new world TRS
                transform_evaluator::TRS world_trs;
                world_trs.translation = translation;
                world_trs.rotation = rotation;
                world_trs.scale = scale;

                // Update world TRS
                transform.SetWorldTRS(std::move(world_trs));
            });

            return true; // Success
        });
}

void TransformServiceCommandList::UpdateWorldTranslation(
    const transform_evaluator::TransformHandle& handle, const XMFLOAT3& translation)
{
    AddCommand(
        [handle, translation](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            transform_evaluator::TransformManager& transform_manager 
                = transform_evaluator::TransformManager::GetInstance();
            transform_manager.WithLock([&](transform_evaluator::TransformManager& manager)
            {
                // Get the Transform using its Handle
                transform_evaluator::Transform& transform = manager.GetTransform(handle);

                // Create new world TRS
                transform_evaluator::TRS world_trs = transform.GetWorldTRS();
                world_trs.translation = translation;

                // Update world TRS
                transform.SetWorldTRS(std::move(world_trs));
            });

            return true; // Success
        });
}

void TransformServiceCommandList::UpdateWorldRotation(
    const transform_evaluator::TransformHandle& handle, const XMFLOAT4& rotation)
{
    AddCommand(
        [handle, rotation](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            transform_evaluator::TransformManager& transform_manager 
                = transform_evaluator::TransformManager::GetInstance();
            transform_manager.WithLock([&](transform_evaluator::TransformManager& manager)
            {
                // Get the Transform using its Handle
                transform_evaluator::Transform& transform = manager.GetTransform(handle);

                // Create new world TRS
                transform_evaluator::TRS world_trs = transform.GetWorldTRS();
                world_trs.rotation = rotation;

                // Update world TRS
                transform.SetWorldTRS(std::move(world_trs));
            });

            return true; // Success
        });
}

void TransformServiceCommandList::UpdateWorldScale(
    const transform_evaluator::TransformHandle& handle, const XMFLOAT3& scale)
{
    AddCommand(
        [handle, scale](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            transform_evaluator::TransformManager& transform_manager 
                = transform_evaluator::TransformManager::GetInstance();
            transform_manager.WithLock([&](transform_evaluator::TransformManager& manager)
            {
                // Get the Transform using its Handle
                transform_evaluator::Transform& transform = manager.GetTransform(handle);

                // Create new world TRS
                transform_evaluator::TRS world_trs = transform.GetWorldTRS();
                world_trs.scale = scale;

                // Update world TRS
                transform.SetWorldTRS(std::move(world_trs));
            });

            return true; // Success
        });
}

void TransformServiceCommandList::DestroyTransform(const transform_evaluator::TransformHandle& handle)
{
    AddCommand(
        [handle](mono_service::ServiceAPI& service_api) -> bool
        {
            // Get graphics service API
            static_assert(
                std::is_base_of<mono_service::ServiceAPI, TransformService>::value,
                "TransformService must be derived from ServiceAPI.");
            TransformServiceAPI& transform_service_api = dynamic_cast<TransformServiceAPI&>(service_api);

            // Erase transform using transform eraser
            transform_service_api.GetTransformEraser().EraseTransform(handle);

            return true; // Success
        });
}

} // namespace mono_transform_service