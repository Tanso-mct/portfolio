#pragma once

#include <memory>
#include <unordered_map>
#include <functional>

#include "mono_cycle/include/dll_config.h"

namespace mono_cycle
{

// State
using State = uint32_t;

// Null State
constexpr State STATE_NULL = 0;

// Event
using Event = uint32_t;

// Null Event
constexpr Event EVENT_NULL = 0;

enum class NextStateType
{
    TO_PREVIOUS, // Back to previous state
    STAY, // Remain in current state
    TO_NEXT, // Proceed to next state
};

template <typename ...Args>
class StateMachine
{
public:
    // Action function type
    // If the action returns false, the state transition will not occur
    using Action = std::function<NextStateType(Args...)>;

    StateMachine() = default;
    ~StateMachine() = default;

    // Add state transition based on an event
    void AddTransition(State fromState, Event event, State toState, Action action, State previousState = STATE_NULL)
    {
        // Add the transition to the map
        transitions_[fromState][event] = Transition{previousState, toState, std::move(action)};
    }

    // Handle an event and perform state transition if applicable
    void HandleEvent(Event event, Args... args)
    {
        // Check if there is a transition for the current state
        auto stateIt = transitions_.find(currentState_);
        assert(stateIt != transitions_.end() && "No transitions defined for current state.");

        // Check if there is a transition for the given event
        auto eventIt = stateIt->second.find(event);
        assert(eventIt != stateIt->second.end() && "No transition defined for this event in current state.");

        // Perform the action if it exists
        const Transition &transition = eventIt->second;

        // Execute action if it exists
        if (transition.action)
        {
            mono_cycle::NextStateType result = transition.action(args...);

            switch (result)
            {
            case mono_cycle::NextStateType::TO_PREVIOUS:
                // Revert to previous state
                currentState_ = transition.previousState;
                return;

            case mono_cycle::NextStateType::STAY:
                // Stay in the current state (no transition)
                return;

            case mono_cycle::NextStateType::TO_NEXT:
                // Proceed with the transition
                currentState_ = transition.toState;
                break;
            }
        }
    }

    // Get the current state
    State GetCurrentState() const { return currentState_; }

private:
    // Struct to represent a state transition
    struct Transition
    {
        State previousState;
        State toState;
        Action action;
    };

    // Current state of the state machine
    State currentState_ = STATE_NULL;

    // Map of state transitions: from State -> (Event -> Transition)
    std::unordered_map<State, std::unordered_map<Event, Transition>> transitions_;
};

} // namespace mono_cycle