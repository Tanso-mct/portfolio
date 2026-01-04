#include "mono_cycle_test/pch.h"

#include "mono_cycle/include/state_machine.h"
#pragma comment(lib, "mono_cycle.lib")

TEST(StateMachine, BasicTransition)
{
    mono_cycle::StateMachine stateMachine;

    // Define test states
    enum class TestState : mono_cycle::State
    {
        STATE_A = 1,    
        STATE_B = 2
    };

    // Define test events
    enum class TestEvent : mono_cycle::Event
    {
        Start = 1,
        Update = 2,
        Finish = 3
    };

    // Add transitions
    stateMachine.AddTransition(
        mono_cycle::STATE_NULL, (mono_cycle::Event)TestEvent::Start, (mono_cycle::State)TestState::STATE_A, 
        []()
    {
        // Action for Start event
        std::cout << "State A Starting..." << std::endl;
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)TestState::STATE_A, (mono_cycle::Event)TestEvent::Update, mono_cycle::STATE_NULL,
        []()
    {
        // Action for Update event
        std::cout << "State A Updating..." << std::endl;
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)TestState::STATE_A, (mono_cycle::Event)TestEvent::Finish, (mono_cycle::State)TestState::STATE_B,
        []()
    {
        // Action for Finish event
        std::cout << "State A Finishing..." << std::endl;
        return mono_cycle::NextStateType::TO_NEXT;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)TestState::STATE_B, (mono_cycle::Event)TestEvent::Start, mono_cycle::STATE_NULL,
        []()
    {
        // Action for Start event in STATE_B
        std::cout << "State B Starting..." << std::endl;
        return mono_cycle::NextStateType::STAY;
    });

    stateMachine.AddTransition(
        (mono_cycle::State)TestState::STATE_B, (mono_cycle::Event)TestEvent::Finish, mono_cycle::STATE_NULL,
        []()
    {
        // Action for Finish event in STATE_B
        std::cout << "State B Finishing..." << std::endl;
        return mono_cycle::NextStateType::TO_PREVIOUS;
        
    }, (mono_cycle::State)TestState::STATE_A);

    // Initial state should be STATE_NULL
    EXPECT_EQ(stateMachine.GetCurrentState(), mono_cycle::STATE_NULL);

    // Handle Start event in STATE_NULL
    stateMachine.HandleEvent((mono_cycle::Event)TestEvent::Start);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)TestState::STATE_A);

    // Handle Update event in STATE_A
    stateMachine.HandleEvent((mono_cycle::Event)TestEvent::Update);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)TestState::STATE_A);

    // Handle Finish event in STATE_A
    stateMachine.HandleEvent((mono_cycle::Event)TestEvent::Finish);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)TestState::STATE_B);

    // Handle Start event in STATE_B
    stateMachine.HandleEvent((mono_cycle::Event)TestEvent::Start);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)TestState::STATE_B);

    // Handle Finish event in STATE_B
    stateMachine.HandleEvent((mono_cycle::Event)TestEvent::Finish);
    EXPECT_EQ(stateMachine.GetCurrentState(), (mono_cycle::State)TestState::STATE_A);
}