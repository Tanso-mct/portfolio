#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/title_system_scheduler.h"

#include "balloon_shooter/include/feature/shared_game_state.h"
#include "balloon_shooter/include/feature/game_state_loading.h"
#include "balloon_shooter/include/feature/game_state_playing.h"
#include "balloon_shooter/include/feature/game_state_title.h"
#include "balloon_shooter/include/feature/game_state_exit.h"

#include "balloon_shooter/include/feature/system_ui_controller.h"
#include "balloon_shooter/include/feature/system_ui_bar.h"
#include "balloon_shooter/include/feature/system_ui_button.h"

#include "wbp_transform/plugin.h"
#pragma comment(lib, "wbp_transform.lib")

#include "wbp_render/plugin.h"
#pragma comment(lib, "wbp_render.lib")

#include "wbp_collision/plugin.h"
#pragma comment(lib, "wbp_collision.lib")

#include "wbp_physics/plugin.h"
#pragma comment(lib, "wbp_physics.lib")

#include "wbp_locator/plugin.h"
#pragma comment(lib, "wbp_locator.lib")

#include "balloon_shooter/include/scene/load_scene.h"

namespace
{

    void LockCursor(UINT clientWidth, UINT clinetHeight, const HWND &windowHandle)
    {
        while (ShowCursor(FALSE) >= 0);

        POINT centerPoint = {clientWidth / 2, clinetHeight / 2};
        ClientToScreen(windowHandle, &centerPoint);

        RECT clip = { centerPoint.x, centerPoint.y, centerPoint.x+1, centerPoint.y+1 };
        ClipCursor(&clip);
        SetCursorPos(centerPoint.x, centerPoint.y);
    }

} // namespace

void balloon_shooter::TitleSystemScheduler::Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args)
{
    // 使用するコンテナを取得
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    if (gameStateSharedFacade->GetGameState() == balloon_shooter::InitialGameStateID())
    {
        // このシーンからゲームが始まったので、初期状態のゲーム状態を更新
        gameStateSharedFacade->SetGameState(balloon_shooter::TitleGameStateID());

        // 初期化時に更新する必要のあるシステムを更新
        systemCont.Get(wbp_locator::LocatorSystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
    }
    else
    {
        size_t beforeState = 0;
        if (gameStateSharedFacade->IsSwitched(beforeState))
        {
            if (gameStateSharedFacade->GetGameState() == balloon_shooter::LoadingGameStateID())
            {
                // ローディング状態に遷移した場合、シーンの遷移を行う
                args.state_ = wb::SceneState::Switching;
                args.nextSceneID_ = balloon_shooter::LoadSceneFacadeID();
                return;
            }
            else if (gameStateSharedFacade->GetGameState() == balloon_shooter::ExitGameStateID())
            {
                // ゲーム終了状態に遷移した場合、アプリケーションを終了する
                args.state_ = wb::SceneState::NeedToExit;
                return;
            }
        }
    }

    systemCont.Get(balloon_shooter::UIControllerSystemID()).Update(args);
    systemCont.Get(balloon_shooter::UIBarSystemID()).Update(args);
    systemCont.Get(balloon_shooter::UIBtnSystemID()).Update(args);

    if (gameStateSharedFacade->GetGameState() == balloon_shooter::TitleGameStateID())
    {
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
        systemCont.Get(wbp_render::RenderSystemID()).Update(args);
    }
}