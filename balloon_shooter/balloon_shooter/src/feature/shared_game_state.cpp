#include "balloon_shooter/src/pch.h"
#include "balloon_shooter/include/feature/shared_game_state.h"

#include "balloon_shooter/include/feature/id_factory_game_state.h"

const size_t &balloon_shooter::InitialGameStateID()
{
    static size_t id = balloon_shooter::GameStateIDFactory().Create();
    return id;
}

const size_t &balloon_shooter::GameStateSharedFacadeID()
{
    static size_t id = wb::IDFactory::CreateSharedFacadeID();
    return id;
}

balloon_shooter::GameStateSharedFacade::GameStateSharedFacade()
{
    gameState_ = balloon_shooter::InitialGameStateID();
}

void balloon_shooter::GameStateSharedFacade::SetGameState(size_t state)
{
    if (gameState_ == state)
    {
        // 既に同じ状態にある場合は何もしない
        return;
    }

    beforeState_ = gameState_;
    gameState_ = state;
    isSwitched_ = true;
}

bool balloon_shooter::GameStateSharedFacade::IsSwitched(size_t &beforeState) const
{
    if (isSwitched_)
    {
        beforeState = beforeState_;
        return true;
    }

    return false; // スイッチされていない場合
}

namespace balloon_shooter
{
    WB_REGISTER_SHARED_FACADE(GameStateSharedFacadeID, GameStateSharedFacade);

} // namespace balloon_shooter
