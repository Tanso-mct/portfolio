#pragma once
#include "windows_base/windows_base.h"

#include "balloon_shooter/include/feature/interfaces/game_state.h"

namespace balloon_shooter
{
    const size_t &PlayingGameStateID();

    class PlayingGameState : public IGameState
    {
    private:
        bool isFirstUIUpdate_ = true;
        
    public:
        PlayingGameState() = default;
        ~PlayingGameState() override = default;

        const size_t &GetID() const override;
        void UIUpdate(const wb::SystemArgument &args) override;
        void Switched(const wb::SystemArgument &args) override;
    };

} // namespace balloon_shooter