#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/shared_game_state.h"

namespace balloon_shooter
{
    const size_t &InitialGameStateID();

    const size_t &GameStateSharedFacadeID();

    class GameStateSharedFacade : public IGameStateSharedFacade
    {
    private:
        size_t gameState_ = 0;

        size_t beforeState_ = 0;
        bool isSwitched_ = false;

        bool isFirstStateInitialized_ = false;
        bool isSceneSwitching_ = false;

    public:
        GameStateSharedFacade();
        ~GameStateSharedFacade() override = default;

        const size_t &GetGameState() const override { return gameState_; }
        void SetGameState(size_t state) override;

        bool IsSwitched(size_t &beforeState) const override;
        void ResetSwitchedFlag() override { isSwitched_ = false; }

        bool IsFirstStateInitialized() override { return isFirstStateInitialized_; }
        void SetFirstStateInitialized(bool isFirst) override { isFirstStateInitialized_ = isFirst; }

        bool IsSceneSwitching() const override { return isSceneSwitching_; }
        void SetIsSceneSwitching(bool isSwitching) override { isSceneSwitching_ = isSwitching; }
    };

} // namespace balloon_shooter