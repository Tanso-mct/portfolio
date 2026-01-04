#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/component_player.h"

const size_t &balloon_shooter::PlayerComponentID()
{
    static size_t id = wb::IDFactory::CreateComponentID();
    return id;
}

const size_t &balloon_shooter::PlayerComponent::GetID() const
{
    return PlayerComponentID();
}

void balloon_shooter::PlayerComponent::SetBodyEntityID(std::unique_ptr<wb::IOptionalValue> bodyEntityId)
{
    bodyEntityId_ = std::move(bodyEntityId);
}

wb::IOptionalValue &balloon_shooter::PlayerComponent::GetBodyEntityID() const
{
    if (bodyEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"PlayerComponentのBodyEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_PLAYER", err);
        wb::ThrowRuntimeError(err);
    }

    return *bodyEntityId_;
}

void balloon_shooter::PlayerComponent::SetCameraEntityID(std::unique_ptr<wb::IOptionalValue> cameraEntityId)
{
    cameraEntityId_ = std::move(cameraEntityId);
}

wb::IOptionalValue &balloon_shooter::PlayerComponent::GetCameraEntityID() const
{
    if (cameraEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"PlayerComponentのCameraEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_PLAYER", err);
        wb::ThrowRuntimeError(err);
    }

    return *cameraEntityId_;
}

void balloon_shooter::PlayerComponent::SetGunEntityID(std::unique_ptr<wb::IOptionalValue> gunEntityId)
{
    gunEntityId_ = std::move(gunEntityId);
}

wb::IOptionalValue &balloon_shooter::PlayerComponent::GetGunEntityID() const
{
    if (gunEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"PlayerComponentのGunEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_PLAYER", err);
        wb::ThrowRuntimeError(err);
    }

    return *gunEntityId_;
}

void balloon_shooter::PlayerComponent::SetRayEntityID(std::unique_ptr<wb::IOptionalValue> rayEntityId)
{
    rayEntityId_ = std::move(rayEntityId);
}

wb::IOptionalValue &balloon_shooter::PlayerComponent::GetRayEntityID() const
{
    if (rayEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"PlayerComponentのRayEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_PLAYER", err);
        wb::ThrowRuntimeError(err);
    }

    return *rayEntityId_;
}

namespace balloon_shooter
{
    WB_REGISTER_COMPONENT(PlayerComponentID(), PlayerComponent);

} // namespace balloon_shooter