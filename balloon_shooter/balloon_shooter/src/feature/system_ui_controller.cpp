#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/system_ui_controller.h"

#include "balloon_shooter/include/feature/shared_game_state.h"

#include "balloon_shooter/include/feature/game_state_playing.h"
#include "balloon_shooter/include/feature/game_state_loading.h"
#include "balloon_shooter/include/feature/game_state_title.h"
#include "balloon_shooter/include/feature/game_state_exit.h"
#include "balloon_shooter/include/feature/game_state_clear.h"


const size_t &balloon_shooter::UIControllerSystemID()
{
    static size_t id = wb::IDFactory::CreateSystemID();
    return id;
}

const size_t &balloon_shooter::UIControllerSystem::GetID() const
{
    return balloon_shooter::UIControllerSystemID();
}

void balloon_shooter::UIControllerSystem::Initialize(wb::IAssetContainer &assetContainer)
{
    gameStates_.clear();
    gameStates_[balloon_shooter::PlayingGameStateID()] = std::make_unique<balloon_shooter::PlayingGameState>();
    gameStates_[balloon_shooter::LoadingGameStateID()] = std::make_unique<balloon_shooter::LoadingGameState>();
    gameStates_[balloon_shooter::TitleGameStateID()] = std::make_unique<balloon_shooter::TitleGameState>();
    gameStates_[balloon_shooter::ExitGameStateID()] = std::make_unique<balloon_shooter::ExitGameState>();
    gameStates_[balloon_shooter::ClearGameStateID()] = std::make_unique<balloon_shooter::ClearGameState>();
}

void balloon_shooter::UIControllerSystem::Update(const wb::SystemArgument &args)
{
    // 使用するコンテナを取得
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    size_t beforeState = balloon_shooter::InitialGameStateID();
    if (gameStateSharedFacade->IsSwitched(beforeState))
    {
        // ゲーム状態が切り替わった場合、UIの切り替えを行う
        if (gameStates_.find(beforeState) != gameStates_.end())
        {
            balloon_shooter::IGameState *beforeGameState = gameStates_[beforeState].get();
            beforeGameState->Switched(args);
        }
        
        // 切り替えフラグをリセット
        gameStateSharedFacade->ResetSwitchedFlag();
    }

    if (gameStates_.find(gameStateSharedFacade->GetGameState()) != gameStates_.end())
    {
        // 現在のゲーム状態を取得
        balloon_shooter::IGameState *currentGameState = gameStates_[gameStateSharedFacade->GetGameState()].get();
        
        // UIの更新を行う
        currentGameState->UIUpdate(args);
    }
}

namespace balloon_shooter
{
    WB_REGISTER_SYSTEM(UIControllerSystem, UIControllerSystemID());

} // namespace balloon_shooter