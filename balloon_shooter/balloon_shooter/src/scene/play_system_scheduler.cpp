#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/scene/play_system_scheduler.h"

#include "balloon_shooter/include/feature/system_ui_controller.h"
#include "balloon_shooter/include/feature/shared_game_state.h"
#include "balloon_shooter/include/feature/game_state_playing.h"
#include "balloon_shooter/include/feature/game_state_loading.h"
#include "balloon_shooter/include/feature/game_state_exit.h"
#include "balloon_shooter/include/feature/game_state_title.h"
#include "balloon_shooter/include/feature/game_state_clear.h"

#include "balloon_shooter/include/feature/system_player.h"
#include "balloon_shooter/include/feature/system_balloon_controller.h"
#include "balloon_shooter/include/feature/system_ui_bar.h"
#include "balloon_shooter/include/feature/system_ui_button.h"

#include "balloon_shooter/include/scene/title_scene.h"
#include "balloon_shooter/include/scene/load_scene.h"

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

void balloon_shooter::PlaySystemScheduler::Execute(wb::ISystemContainer &systemCont, wb::SystemArgument &args)
{
    // 使用するコンテナを取得
    wb::ISharedContainer &sharedContainer = args.containerStorage_.GetContainer<wb::ISharedContainer>();
    wb::IWindowContainer &windowContainer = args.containerStorage_.GetContainer<wb::IWindowContainer>();
    wb::IMonitorContainer &monitorContainer = args.containerStorage_.GetContainer<wb::IMonitorContainer>();

    wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);

    // キーボードとマウスのモニターを取得
    wb::IKeyboardMonitor *keyboardMonitor = nullptr;
    wb::IMouseMonitor *mouseMonitor = nullptr;
    for (const size_t &monitorID : window.GetMonitorIDs())
    {
        wb::IMonitor &monitor = monitorContainer.Get(monitorID);
        if (keyboardMonitor == nullptr) keyboardMonitor = wb::As<wb::IKeyboardMonitor>(&monitor);
        if (mouseMonitor == nullptr) mouseMonitor = wb::As<wb::IMouseMonitor>(&monitor);
    }

    // GameStateSharedFacadeを取得
    wb::ISharedFacade &sharedFacade = sharedContainer.Get(balloon_shooter::GameStateSharedFacadeID());
    balloon_shooter::IGameStateSharedFacade *gameStateSharedFacade = wb::As<balloon_shooter::IGameStateSharedFacade>(&sharedFacade);

    if (keyboardMonitor != nullptr && keyboardMonitor->GetKeyDown(wb::KeyCode::Escape))
    {
        // Escキーが押された場合、ゲーム状態を終了状態に変更
        gameStateSharedFacade->SetGameState(balloon_shooter::ExitGameStateID());
    }

    if 
    (
        gameStateSharedFacade->GetGameState() == balloon_shooter::InitialGameStateID() && 
        !gameStateSharedFacade->IsFirstStateInitialized() && !gameStateSharedFacade->IsSceneSwitching()
    ){
        // このシーンからゲームが始まったので、初期状態のゲーム状態を更新
        gameStateSharedFacade->SetGameState(balloon_shooter::PlayingGameStateID());

        // カーソルをロック
        wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);
        LockCursor(window.GetClientWidth(), window.GetClientHeight(), window.GetHandle());

        // 初期化時に更新する必要のあるシステムを更新
        systemCont.Get(wbp_locator::LocatorSystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

        // 最初の状態が初期化されたことをフラグで記録
        gameStateSharedFacade->SetFirstStateInitialized(true);
    }
    else if 
    (
        gameStateSharedFacade->GetGameState() == balloon_shooter::LoadingGameStateID() && 
        !gameStateSharedFacade->IsFirstStateInitialized() && !gameStateSharedFacade->IsSceneSwitching()
    ){
        // ローディング状態からゲームプレイ状態に移行
        gameStateSharedFacade->SetGameState(balloon_shooter::PlayingGameStateID());

        // カーソルをロック
        wb::IWindowFacade &window = windowContainer.Get(args.belongWindowID_);
        LockCursor(window.GetClientWidth(), window.GetClientHeight(), window.GetHandle());

        // 初期化時に更新する必要のあるシステムを更新
        systemCont.Get(wbp_locator::LocatorSystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

        // 最初の状態が初期化されたことをフラグで記録
        gameStateSharedFacade->SetFirstStateInitialized(true);
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
                gameStateSharedFacade->SetIsSceneSwitching(true);

                // 最初の状態は再度初期化されるため、フラグをリセット
                gameStateSharedFacade->SetFirstStateInitialized(false);

                return;
            }
            else if (gameStateSharedFacade->GetGameState() == balloon_shooter::ExitGameStateID())
            {
                // ゲーム終了状態に遷移した場合、アプリケーションを終了する
                args.state_ = wb::SceneState::NeedToExit;
                return;
            }
            else if (gameStateSharedFacade->GetGameState() == balloon_shooter::TitleGameStateID())
            {
                // ゲームクリア状態に遷移した場合、シーンの遷移を行う
                args.state_ = wb::SceneState::Switching;
                args.nextSceneID_ = balloon_shooter::TitleSceneFacadeID();
                gameStateSharedFacade->SetIsSceneSwitching(true);

                // 最初の状態は再度初期化されるため、フラグをリセット
                gameStateSharedFacade->SetFirstStateInitialized(false);

                return;
            }
        }
    }

    systemCont.Get(balloon_shooter::UIControllerSystemID()).Update(args);
    systemCont.Get(balloon_shooter::UIBarSystemID()).Update(args);
    systemCont.Get(balloon_shooter::UIBtnSystemID()).Update(args);

    if (gameStateSharedFacade->GetGameState() == balloon_shooter::PlayingGameStateID())
    {
        systemCont.Get(balloon_shooter::BalloonControllerSystemID()).Update(args);
        systemCont.Get(balloon_shooter::PlayerSystemID()).Update(args);

        systemCont.Get(wbp_physics::RigidBodySystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

        systemCont.Get(wbp_collision::CollisionSystemID()).Update(args);
        systemCont.Get(wbp_physics::RigidBodyResponseSystemID()).Update(args);
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);

        systemCont.Get(wbp_render::RenderSystemID()).Update(args);
    }
    else if (gameStateSharedFacade->GetGameState() == balloon_shooter::ClearGameStateID())
    {
        systemCont.Get(wbp_transform::TransformSystemID()).Update(args);
        systemCont.Get(wbp_render::RenderSystemID()).Update(args);
    }
}