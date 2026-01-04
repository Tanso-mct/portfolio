#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/game_state_loading.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"
#include "balloon_shooter/include/feature/component_ui_load.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

const size_t &balloon_shooter::LoadingGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory::Create();
    return id;
}

const size_t &balloon_shooter::LoadingGameState::GetID() const
{
    return balloon_shooter::LoadingGameStateID();
}

void balloon_shooter::LoadingGameState::UIUpdate(const wb::SystemArgument &args)
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

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::LoadUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::LoadUIBackgroundComponentID()
        );
    }
}

void balloon_shooter::LoadingGameState::Switched(const wb::SystemArgument &args)
{
    isFirstUIUpdate_ = true;

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::LoadUIBackgroundComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::LoadUIBackgroundComponentID()
        );
    }
}