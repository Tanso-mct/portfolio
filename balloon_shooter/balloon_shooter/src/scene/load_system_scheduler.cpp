#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/load_system_scheduler.h"

#include "balloon_shooter/include/feature/shared_game_state.h"
#include "balloon_shooter/include/feature/game_state_loading.h"
#include "balloon_shooter/include/feature/game_state_playing.h"
#include "balloon_shooter/include/feature/game_state_title.h"

#include "balloon_shooter/include/feature/system_ui_controller.h"

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "balloon_shooter/include/scene/play_scene.h"

void balloon_shooter::LoadSystemScheduler::Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args)
{
    // 使用するコンテナを取得
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    gameStateSharedFacade->SetIsSceneSwitching(false);
    gameStateSharedFacade->SetFirstStateInitialized(false);

    if (gameStateSharedFacade->GetGameState() == balloon_shooter::InitialGameStateID())
    {
        // このシーンからゲームが始まったので、初期状態のゲーム状態を更新
        gameStateSharedFacade->SetGameState(balloon_shooter::LoadingGameStateID());
    }

    systemCont.Get(balloon_shooter::UIControllerSystemID()).Update(args);

    systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
    systemCont.Get(wbp_render::RenderSystemID()).Update(args);

    args.state_ = wb::SceneState::Switching;
    args.nextSceneID_ = balloon_shooter::PlaySceneFacadeID();
}