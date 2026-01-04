#include "example/src/pch.h"
#include "example/include/feature/component_controller.h"

const size_t &example::ControllerComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &example::ControllerComponent::GetID() const
{
    return ControllerComponentID();
}

void example::ControllerComponent::SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId)
{
    if (bodyEntityId == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Body entity ID cannot be null."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("EXAMPLE", err);
        wb::ThrowRuntimeError(err);
    }

    bodyEntityId_ = std::move(bodyEntityId);
}

wb::IOptionalValue &example::ControllerComponent::GetBodyEntityID() const
{
    if (bodyEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Body entity ID is not set."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("EXAMPLE", err);
        wb::ThrowRuntimeError(err);
    }

    return *bodyEntityId_;
}

void example::ControllerComponent::SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId)
{
    if (cameraEntityId == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Camera entity ID cannot be null."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("EXAMPLE", err);
        wb::ThrowRuntimeError(err);
    }

    cameraEntityId_ = std::move(cameraEntityId);
}

wb::IOptionalValue &example::ControllerComponent::GetCameraEntityID() const
{
    if (cameraEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "Camera entity ID is not set."
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("EXAMPLE", err);
        wb::ThrowRuntimeError(err);
    }

    return *cameraEntityId_;
}

namespace example
{
    WB_REGISTER_COMPONENT(ControllerComponentID(), ControllerComponent);

} // namespace example