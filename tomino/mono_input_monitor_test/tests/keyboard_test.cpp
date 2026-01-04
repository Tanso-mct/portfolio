#include "mono_input_monitor_test/pch.h"

#include "mono_input_monitor/include/keyboard_monitor.h"
#pragma comment(lib, "mono_input_monitor.lib")

TEST(Keyboard, KeyCodeConverter)
{
    mono_input_monitor::KeyCodeConverter converter;

    // Normal lparam
    LPARAM normalLParam = 0;

    EXPECT_EQ(converter.Convert('A', normalLParam), mono_input_monitor::KeyCode::A);
    EXPECT_EQ(converter.Convert('B', normalLParam), mono_input_monitor::KeyCode::B);
    EXPECT_EQ(converter.Convert(VK_RETURN, normalLParam), mono_input_monitor::KeyCode::Return);
    EXPECT_EQ(converter.Convert(VK_ESCAPE, normalLParam), mono_input_monitor::KeyCode::Escape);
    EXPECT_EQ(converter.Convert(VK_SPACE, normalLParam), mono_input_monitor::KeyCode::Space);
    EXPECT_EQ(converter.Convert(VK_TAB, normalLParam), mono_input_monitor::KeyCode::Tab);

    // Extended lparam
    LPARAM extendedLParam = 0;
    extendedLParam |= (1 << 24); // Set the extended key flag

    EXPECT_EQ(converter.Convert(VK_MENU, extendedLParam), mono_input_monitor::KeyCode::RAlt);
}

TEST(Keyboard, KeyInputConverter)
{
    mono_input_monitor::KeyInputConverter converter;

    EXPECT_EQ(converter.Convert(WM_KEYDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_KEYUP), mono_input_monitor::InputType::Up);
    EXPECT_EQ(converter.Convert(WM_SYSKEYDOWN), mono_input_monitor::InputType::Down);
    EXPECT_EQ(converter.Convert(WM_SYSKEYUP), mono_input_monitor::InputType::Up);
}

TEST(Keyboard, InputState)
{
    mono_input_monitor::KeyboardInputState inputState;

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::KeyCode::A);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::KeyCode::A)], mono_input_monitor::InputType::Down);

    mono_input_monitor::UpdateInputState(inputState);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::KeyCode::A)], mono_input_monitor::InputType::Press);

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::KeyCode::A);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::KeyCode::A)], mono_input_monitor::InputType::Up);

    mono_input_monitor::UpdateInputState(inputState);
    EXPECT_EQ(inputState.inputStates_[static_cast<size_t>(mono_input_monitor::KeyCode::A)], mono_input_monitor::InputType::None);
}

TEST(Keyboard, GetKeyStates)
{
    mono_input_monitor::KeyboardInputState inputState;

    // Simulate key down
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::KeyCode::A);
    EXPECT_TRUE(mono_input_monitor::GetKey(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_TRUE(mono_input_monitor::GetKeyDown(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyUp(inputState, mono_input_monitor::KeyCode::A));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate key press
    EXPECT_TRUE(mono_input_monitor::GetKey(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyDown(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyUp(inputState, mono_input_monitor::KeyCode::A));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate key up
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::KeyCode::A);
    EXPECT_FALSE(mono_input_monitor::GetKey(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyDown(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_TRUE(mono_input_monitor::GetKeyUp(inputState, mono_input_monitor::KeyCode::A));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate key down and then up again
    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Down, mono_input_monitor::KeyCode::A);
    mono_input_monitor::UpdateInputState(inputState);

    mono_input_monitor::EditInputState(inputState, mono_input_monitor::InputType::Up, mono_input_monitor::KeyCode::A);
    EXPECT_FALSE(mono_input_monitor::GetKey(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyDown(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_TRUE(mono_input_monitor::GetKeyUp(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_TRUE(mono_input_monitor::GetKeyDoubleTap(inputState, mono_input_monitor::KeyCode::A));

    mono_input_monitor::UpdateInputState(inputState);

    // Simulate key reset
    EXPECT_FALSE(mono_input_monitor::GetKey(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyDown(inputState, mono_input_monitor::KeyCode::A));
    EXPECT_FALSE(mono_input_monitor::GetKeyUp(inputState, mono_input_monitor::KeyCode::A));
}