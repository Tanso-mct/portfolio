#pragma once
#include "bdc_game_flow/include/dll_config.h"

#include "mono_cycle/include/state_machine.h"
#include "riaecs/riaecs.h"
#include "bdc_game_flow/include/state_machine_holder.h"

namespace bdc_game_flow
{
    // Game flow states
    enum class GameFlowState : mono_cycle::State
    {
        START = 1,
        PHASE_1,
        PHASE_2,
        PHASE_3,
        GAME_OVER,
        GAME_CLEAR,
        RE_SPAWN,
        MENU
    };

    // Game flow events
    enum class GameFlowEvent : mono_cycle::Event
    {
        START = 1,
        PAUSE,
        RESUME,
        DOT_GET,
        DOT_COLLECT_COUNT_REACHED,
        CRYSTAL_GET,
        CRYSTAL_COLLECT_COUNT_REACHED,
        PLAYER_DEAD,
        PLAYER_DEAD_COUNT_REACHED,
        RE_SPAWN_COMPLETE,
        BACK_TO_TITLE
    };

    constexpr uint32_t DEFAULT_NEED_DOT_COUNT_TO_SPAWN_CRYSTAL = 20;
    constexpr uint32_t DEFAULT_NEED_CRYSTAL_COUNT_TO_CLEAR_PHASE = 2;
    constexpr uint32_t DEFAULT_NEED_DEAD_COUNT_TO_GAME_OVER = 3;

    constexpr size_t ComponentGameFlowMaxCount = 1000;
    class BDC_GAME_FLOW_API ComponentGameFlow :
        public StateMachineHolder  
    {
    public:
        ComponentGameFlow();
        ~ComponentGameFlow();

        struct SetupParam
        {
            // Number of dots needed to spawn a crystal
            uint32_t needDotCountToSpawnCrystal = DEFAULT_NEED_DOT_COUNT_TO_SPAWN_CRYSTAL;

            // Number of crystals needed to clear a phase
            uint32_t needCrystalCountToClearPhase = DEFAULT_NEED_CRYSTAL_COUNT_TO_CLEAR_PHASE;

            // Number of deaths needed to trigger game over
            uint32_t needDeadCountToGameOver = DEFAULT_NEED_DEAD_COUNT_TO_GAME_OVER;
        };
        void Setup(SetupParam &param);

        // Get number of dots needed to spawn a crystal
        uint32_t GetNeedDotCountToSpawnCrystal() const { return needDotCountToSpawnCrystal_; }

        // Get number of crystals needed to clear a phase
        uint32_t GetNeedCrystalCountToClearPhase() const { return needCrystalCountToClearPhase_; }

        // Get number of deaths needed to trigger game over
        uint32_t GetNeedDeadCountToGameOver() const { return needDeadCountToGameOver_; }

        // Set current system loop command
        void SetCurrentSystemLoopCommand(std::unique_ptr<riaecs::ISystemLoopCommand> cmd)
        {
            currentSystemLoopCommand_ = std::move(cmd);
        }

        // Get current system loop command
        riaecs::ISystemLoopCommand* GetCurrentSystemLoopCommand() const
        {
            if (!currentSystemLoopCommand_)
                return nullptr;

            return currentSystemLoopCommand_.get();
        }

    private:
        // Number of dots needed to spawn a crystal
        uint32_t needDotCountToSpawnCrystal_ = DEFAULT_NEED_DOT_COUNT_TO_SPAWN_CRYSTAL;

        // Number of crystals needed to clear a phase
        uint32_t needCrystalCountToClearPhase_ = DEFAULT_NEED_CRYSTAL_COUNT_TO_CLEAR_PHASE;

        // Number of deaths needed to trigger game over
        uint32_t needDeadCountToGameOver_ = DEFAULT_NEED_DEAD_COUNT_TO_GAME_OVER;

        // Current system loop command
        std::unique_ptr<riaecs::ISystemLoopCommand> currentSystemLoopCommand_ = nullptr;
    };

    extern BDC_GAME_FLOW_API riaecs::ComponentRegistrar
    <ComponentGameFlow, ComponentGameFlowMaxCount> ComponentGameFlowID;

} // namespace bdc_game_flow