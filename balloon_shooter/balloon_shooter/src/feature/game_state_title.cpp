#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/game_state_title.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"
#include "balloon_shooter/include/feature/component_ui_title.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

const size_t &balloon_shooter::TitleGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory::Create();
    return id;
}

const size_t &balloon_shooter::TitleGameState::GetID() const
{
    return balloon_shooter::TitleGameStateID();
}

void balloon_shooter::TitleGameState::UIUpdate(const wb::SystemArgument &args)
{
    if (!isFirstUIUpdate_)
    {
        // 初回のUI更新ではない場合は何もしない
        return;
    }
    isFirstUIUpdate_ = false;

    // カーソルを表示
    while (ShowCursor(TRUE) < 0);
    ClipCursor(nullptr);

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::TitleUIBackgroundComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::TitleUIPlayBtnComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIExitBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::TitleUIExitBtnComponentID()
        );
    }
}

void balloon_shooter::TitleGameState::Switched(const wb::SystemArgument &args)
{
    isFirstUIUpdate_ = true;

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::TitleUIBackgroundComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::TitleUIPlayBtnComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::TitleUIExitBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::TitleUIExitBtnComponentID()
        );
    }
}
