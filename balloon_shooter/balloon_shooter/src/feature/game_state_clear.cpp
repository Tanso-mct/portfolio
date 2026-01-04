#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/game_state_clear.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"
#include "balloon_shooter/include/feature/component_ui_clear.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

const size_t &balloon_shooter::ClearGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory::Create();
    return id;
}

const size_t &balloon_shooter::ClearGameState::GetID() const
{
    return balloon_shooter::ClearGameStateID();
}

void balloon_shooter::ClearGameState::UIUpdate(const wb::SystemArgument &args)
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

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::ClearUIBackgroundComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::ClearUIPlayBtnComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIMenuBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::ClearUIMenuBtnComponentID()
        );
    }
}

void balloon_shooter::ClearGameState::Switched(const wb::SystemArgument &args)
{
    isFirstUIUpdate_ = true;

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::ClearUIBackgroundComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIPlayBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::ClearUIPlayBtnComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::ClearUIMenuBtnComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIButton
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::ClearUIMenuBtnComponentID()
        );
    }
}
