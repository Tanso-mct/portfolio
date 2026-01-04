#include "mono_service/src/pch.h"
#include "mono_transform_service/include/transform_service_view.h"

namespace mono_transform_service
{

TransformServiceView::TransformServiceView(const mono_service::ServiceAPI &service_api) :
    mono_service::ServiceView(service_api)
{
}

DirectX::XMMATRIX TransformServiceView::GetWorldMatrix(const transform_evaluator::TransformHandle& handle) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, TransformServiceAPI>::value,
        "TransformServiceAPI must be derived from ServiceAPI.");
    const TransformServiceAPI& transform_service_api = dynamic_cast<const TransformServiceAPI&>(service_api_);

    // Return the world matrix
    return transform_service_api.GetWorldMatrix(handle);
}

DirectX::XMFLOAT3 TransformServiceView::GetTranslation(const transform_evaluator::TransformHandle& handle) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, TransformServiceAPI>::value,
        "TransformServiceAPI must be derived from ServiceAPI.");
    const TransformServiceAPI& transform_service_api = dynamic_cast<const TransformServiceAPI&>(service_api_);

    // Return the translation
    return transform_service_api.GetTranslation(handle);
}

DirectX::XMFLOAT4 TransformServiceView::GetRotation(const transform_evaluator::TransformHandle& handle) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, TransformServiceAPI>::value,
        "TransformServiceAPI must be derived from ServiceAPI.");
    const TransformServiceAPI& transform_service_api = dynamic_cast<const TransformServiceAPI&>(service_api_);

    // Return the rotation
    return transform_service_api.GetRotation(handle);
}

DirectX::XMFLOAT3 TransformServiceView::GetScale(const transform_evaluator::TransformHandle& handle) const
{
    static_assert(
        std::is_base_of<mono_service::ServiceAPI, TransformServiceAPI>::value,
        "TransformServiceAPI must be derived from ServiceAPI.");
    const TransformServiceAPI& transform_service_api = dynamic_cast<const TransformServiceAPI&>(service_api_);

    // Return the scale
    return transform_service_api.GetScale(handle);
}

} // namespace mono_transform_service