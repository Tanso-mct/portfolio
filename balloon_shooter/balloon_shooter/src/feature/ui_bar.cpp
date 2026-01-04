#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/ui_bar.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

void balloon_shooter::UIBarComponent::SetFrameEntityID(std::unique_ptr<wb::IOptionalValue> frameEntityId)
{
    frameEntityId_ = std::move(frameEntityId);
}

wb::IOptionalValue &balloon_shooter::UIBarComponent::GetFrameEntityID() const
{
    if (frameEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"UIBarComponentのFrameEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_UI", err);
        wb::ThrowRuntimeError(err);
    }

    return *frameEntityId_;
}

void balloon_shooter::UIBarComponent::SetBarEntityID(std::unique_ptr<wb::IOptionalValue> barEntityId)
{
    barEntityId_ = std::move(barEntityId);
}

wb::IOptionalValue &balloon_shooter::UIBarComponent::GetBarEntityID() const
{
    if (barEntityId_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"UIBarComponentのBarEntityIDが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_UI", err);
        wb::ThrowRuntimeError(err);
    }

    return *barEntityId_;
}

void balloon_shooter::ShowUIBar
(
    wb::IEntity *entity, wb::IComponentContainer &componentCont, wb::IEntityContainer &entityCont,
    bool isShow, size_t componentID
){
    wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
    wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
    if (identity == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "UIBarComponentを継承しているComponentはIdentityComponentを持っていません。",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_GAME_FLOW", err);
        wb::ThrowRuntimeError(err);
    }

    identity->SetActiveSelf(isShow);

    // UIBarComponentを継承しているComponentの表示
    wb::IComponent *uiBarComponent = entity->GetComponent(componentID, componentCont);
    balloon_shooter::IUIBarComponent *uiBar = wb::As<balloon_shooter::IUIBarComponent>(uiBarComponent);

    wb::IEntity *frameEntity = entityCont.PtrGet(uiBar->GetFrameEntityID());
    wb::IComponent *frameIdentityComponent = frameEntity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
    wbp_identity::IIdentityComponent *frameIdentity = wb::As<wbp_identity::IIdentityComponent>(frameIdentityComponent);
    if (frameIdentity == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "UIBarComponentを継承しているComponentのFrameEntityはIdentityComponentを持っていません。",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_GAME_FLOW", err);
        wb::ThrowRuntimeError(err);
    }
    frameIdentity->SetActiveSelf(isShow);

    wb::IEntity *barEntity = entityCont.PtrGet(uiBar->GetBarEntityID());
    wb::IComponent *barIdentityComponent = barEntity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
    wbp_identity::IIdentityComponent *barIdentity = wb::As<wbp_identity::IIdentityComponent>(barIdentityComponent);
    if (barIdentity == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {
                "UIBarComponentを継承しているComponentのBarEntityはIdentityComponentを持っていません。",
            }
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_GAME_FLOW", err);
        wb::ThrowRuntimeError(err);
    }
    barIdentity->SetActiveSelf(isShow);
}