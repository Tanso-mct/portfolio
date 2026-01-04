#include "mono_input_monitor_test/pch.h"

#include "mono_input_monitor/include/mouse_monitor.h"
#pragma comment(lib, "mono_input_monitor.lib")

TEST(Mouse, MouseCodeConverter)
{
    mono_input_monitor::MouseCodeConverter converter;

    // Test X button codes
    WPARAM notXButton = 0;
    WPARAM xButton1 = (XBUTTON1 << 16) | 0;
    WPARAM xButton2 = (XBUTTON2 << 16) | 0;

    // Normal mouse messages
    EXPECT_EQ(converter.Convert(WM_LBUTTONDOWN, notXButton), mono_input_monitor::MouseCode::Left);
    EXPECT_EQ(converter.Convert(WM_RBUTTONDOWN, notXButton), mono_input_monitor::MouseCode::Right);
    EXPECT_EQ(converter.Convert(WM_MBUTTONDOWN, notXButton), mono_input_monitor::MouseCode::Middle);
    EXPECT_EQ(converter.Convert(WM_XBUTTONDOWN, xButton1), mono_input_monitor::MouseCode::X1);
    EXPECT_EQ(converter.Convert(WM_XBUTTONDOWN, xButton2), mono_input_monitor::MouseCode::X2);
    EXPECT_EQ(converter.Convert(WM_MOUSEWHEEL, notXButton), mono_input_monitor::MouseCode::Wheel);
    EXPECT_EQ(converter.Convert(WM_MOUSEMOVE, notXButton), mono_input_monitor::MouseCode::Move);
}

TEST(Mouse, MouseInputConverter)
{
    mono_input_monitor::MouseInputConverter converter;

    EXPECT_EQ(converter.Convert(WM_LBUTTONDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_LBUTTONUP), mono_input_monitor::InputType::Up);
    EXPECT_EQ(converter.Convert(WM_RBUTTONDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_RBUTTONUP), mono_input_monitor::InputType::Up);
    EXPECT_EQ(converter.Convert(WM_MBUTTONDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_MBUTTONUP), mono_input_monitor::InputType::Up);
    EXPECT_EQ(converter.Convert(WM_XBUTTONDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_XBUTTONUP), mono_input_monitor::InputType::Up);
}

TEST(Mouse, InputState)
{
    mono_input_monitor::MouseInputState inputState;
    WPARAM wParam = 0;
    LPARAM lParam = 0;

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::MouseCode::Left, wParam, lParam);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::MouseCode::Left)], mono_input_monitor::InputType::Down);

    mono_input_monitor::UpdateInputState(inputState);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::MouseCode::Left)], mono_input_monitor::InputType::Press);

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::MouseCode::Left, wParam, lParam);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::MouseCode::Left)], mono_input_monitor::InputType::Up);

    mono_input_monitor::UpdateInputState(inputState);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::MouseCode::Left)], mono_input_monitor::InputType::None);
}

TEST(Mouse, GetButtonStates)
{
    mono_input_monitor::MouseInputState inputState;
    WPARAM wParam = 0;
    LPARAM lParam = 0;

    // Simulate button down
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::MouseCode::Left, wParam, lParam);
    EXPECT_TRUE(mono_input_monitor::GetButton(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_TRUE(mono_input_monitor::GetButtonDown(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonUp(inputState, mono_input_monitor::MouseCode::Left));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate button press
    EXPECT_TRUE(mono_input_monitor::GetButton(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonDown(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonUp(inputState, mono_input_monitor::MouseCode::Left));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate button up
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::MouseCode::Left, wParam, lParam);
    EXPECT_FALSE(mono_input_monitor::GetButton(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonDown(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_TRUE(mono_input_monitor::GetButtonUp(inputState, mono_input_monitor::MouseCode::Left));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate button down and then up again
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::MouseCode::Left, wParam, lParam);
    mono_input_monitor::UpdateInputState(inputState);

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::MouseCode::Left, wParam, lParam);
    EXPECT_FALSE(mono_input_monitor::GetButton(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonDown(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_TRUE(mono_input_monitor::GetButtonUp(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_TRUE(mono_input_monitor::GetButtonDoubleTap(inputState, mono_input_monitor::MouseCode::Left, 0.3));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate button reset
    EXPECT_FALSE(mono_input_monitor::GetButton(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonDown(inputState, mono_input_monitor::MouseCode::Left));
    EXPECT_FALSE(mono_input_monitor::GetButtonUp(inputState, mono_input_monitor::MouseCode::Left));
}