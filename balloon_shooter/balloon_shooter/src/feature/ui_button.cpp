#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/ui_button.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

balloon_shooter::IButtonEvent &balloon_shooter::UIButtonComponent::GetEvent()
{
    if (event_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"UIButtonComponentのイベントが設定されていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_UI", err);
        wb::ThrowRuntimeError(err);
    }
    
    return *event_;
}

void balloon_shooter::ShowUIButton(wb::IEntity *entity, wb::IComponentContainer &componentCont, wb::IEntityContainer &entityCont, bool isShow, size_t componentID)
{
    wb::IComponent *identityComponent = entity->GetComponent(wbp_identity::IdentityComponentID(), componentCont);
    wbp_identity::IIdentityComponent *identity = wb::As<wbp_identity::IIdentityComponent>(identityComponent);
    if (identity == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"UIButtonComponentを継承するComponentがIdentityComponentを持っていません。"}
        );
        wb::ConsoleLogErr(err);
        wb::ErrorNotify("BDC_UI", err);
        wb::ThrowRuntimeError(err);
    }

    identity->SetActiveSelf(isShow);
}