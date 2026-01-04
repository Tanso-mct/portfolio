#pragma once

#include "riaecs/riaecs.h"

#include "mono_cycle/include/state_machine.h"
#pragma comment(lib, "mono_cycle.lib")

namespace bdc_game_flow
{
    using GameStateMachine = mono_cycle::StateMachine<riaecs::IECSWorld&>;

    // Holder for a state machine
    class StateMachineHolder
    {
    public:
        virtual ~StateMachineHolder() = default;

        // Accessor for the state machine
        GameStateMachine &GetStateMachine() { return stateMachine; }

    private:
        // State machine instance
        GameStateMachine stateMachine;
    };

} // namespace bdc_game_flow