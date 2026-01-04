#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/game_state_playing.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"

#include "balloon_shooter/include/feature/component_ui_player.h"

#include "wbp_identity/plugin.h"
#pragma comment(lib, "wbp_identity.lib")

const size_t &balloon_shooter::PlayingGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory::Create();
    return id;
}

const size_t &balloon_shooter::PlayingGameState::GetID() const
{
    return balloon_shooter::PlayingGameStateID();
}

void balloon_shooter::PlayingGameState::UIUpdate(const wb::SystemArgument &args)
{
    if (!isFirstUIUpdate_)
    {
        // 初回のUI更新ではない場合は何もしない
        return;
    }
    isFirstUIUpdate_ = false;

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerUIBalloonLeftComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIBar
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::PlayerUIBalloonLeftComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerUIComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            true, balloon_shooter::PlayerUIComponentID()
        );
    }
}

void balloon_shooter::PlayingGameState::Switched(const wb::SystemArgument &args)
{
    isFirstUIUpdate_ = true;

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerUIBalloonLeftComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIBar
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::PlayerUIBalloonLeftComponentID()
        );
    }

    for (const std::unique_ptr<wb::IOptionalValue> &id : (args.entityIDView_)(balloon_shooter::PlayerUIComponentID()))
    {
        wb::IEntity *entity = args.entityContainer_.PtrGet(*id);
        if (entity == nullptr) continue;

        balloon_shooter::ShowUIImage
        (
            entity, args.componentContainer_, args.entityContainer_,
            false, balloon_shooter::PlayerUIComponentID()
        );
    }
}
