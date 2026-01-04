#include "mono_input_monitor/src/pch.h"
#include "mono_input_monitor/include/keyboard_monitor.h"

#pragma comment(lib, "riaecs.lib")

mono_input_monitor::KeyInputConverter::KeyInputConverter()
{
    inputTypeMap_[WM_KEYDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_KEYUP] = mono_input_monitor::InputType::Up;
    
    inputTypeMap_[WM_SYSKEYDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_SYSKEYUP] = mono_input_monitor::InputType::Up;
}

mono_input_monitor::InputType mono_input_monitor::KeyInputConverter::Convert(UINT msg) const
{
    auto it = inputTypeMap_.find(static_cast<UINT>(msg));
    if (it != inputTypeMap_.end())
        return it->second;

    return mono_input_monitor::InputType::None;
}

mono_input_monitor::KeyCodeConverter::KeyCodeConverter()
{
    keyCodeMap_[{'A', false}] = mono_input_monitor::KeyCode::A;
    keyCodeMap_[{'B', false}] = mono_input_monitor::KeyCode::B;
    keyCodeMap_[{'C', false}] = mono_input_monitor::KeyCode::C;
    keyCodeMap_[{'D', false}] = mono_input_monitor::KeyCode::D;
    keyCodeMap_[{'E', false}] = mono_input_monitor::KeyCode::E;
    keyCodeMap_[{'F', false}] = mono_input_monitor::KeyCode::F;
    keyCodeMap_[{'G', false}] = mono_input_monitor::KeyCode::G;
    keyCodeMap_[{'H', false}] = mono_input_monitor::KeyCode::H;
    keyCodeMap_[{'I', false}] = mono_input_monitor::KeyCode::I;
    keyCodeMap_[{'J', false}] = mono_input_monitor::KeyCode::J;
    keyCodeMap_[{'K', false}] = mono_input_monitor::KeyCode::K;
    keyCodeMap_[{'L', false}] = mono_input_monitor::KeyCode::L;
    keyCodeMap_[{'M', false}] = mono_input_monitor::KeyCode::M;
    keyCodeMap_[{'N', false}] = mono_input_monitor::KeyCode::N;
    keyCodeMap_[{'O', false}] = mono_input_monitor::KeyCode::O;
    keyCodeMap_[{'P', false}] = mono_input_monitor::KeyCode::P;
    keyCodeMap_[{'Q', false}] = mono_input_monitor::KeyCode::Q;
    keyCodeMap_[{'R', false}] = mono_input_monitor::KeyCode::R;
    keyCodeMap_[{'S', false}] = mono_input_monitor::KeyCode::S;
    keyCodeMap_[{'T', false}] = mono_input_monitor::KeyCode::T;
    keyCodeMap_[{'U', false}] = mono_input_monitor::KeyCode::U;
    keyCodeMap_[{'V', false}] = mono_input_monitor::KeyCode::V;
    keyCodeMap_[{'W', false}] = mono_input_monitor::KeyCode::W;
    keyCodeMap_[{'X', false}] = mono_input_monitor::KeyCode::X;
    keyCodeMap_[{'Y', false}] = mono_input_monitor::KeyCode::Y;
    keyCodeMap_[{'Z', false}] = mono_input_monitor::KeyCode::Z;

    keyCodeMap_[{VK_RETURN, false}] = mono_input_monitor::KeyCode::Return;
    keyCodeMap_[{VK_ESCAPE, false}] = mono_input_monitor::KeyCode::Escape;
    keyCodeMap_[{VK_SPACE, false}] = mono_input_monitor::KeyCode::Space;
    keyCodeMap_[{VK_TAB, false}] = mono_input_monitor::KeyCode::Tab;
    keyCodeMap_[{VK_BACK, false}] = mono_input_monitor::KeyCode::BackSpace;

    keyCodeMap_[{VK_MENU, true}] = mono_input_monitor::KeyCode::RAlt;
    keyCodeMap_[{VK_MENU, false}] = mono_input_monitor::KeyCode::LAlt;
    keyCodeMap_[{VK_SHIFT, true}] = mono_input_monitor::KeyCode::RShift;
    keyCodeMap_[{VK_SHIFT, false}] = mono_input_monitor::KeyCode::LShift;
    keyCodeMap_[{VK_CONTROL, true}] = mono_input_monitor::KeyCode::RCtrl;
    keyCodeMap_[{VK_CONTROL, false}] = mono_input_monitor::KeyCode::LCtrl;

    keyCodeMap_[{VK_UP, false}] = mono_input_monitor::KeyCode::Up;
    keyCodeMap_[{VK_DOWN, false}] = mono_input_monitor::KeyCode::Down;
    keyCodeMap_[{VK_LEFT, false}] = mono_input_monitor::KeyCode::Left;
    keyCodeMap_[{VK_RIGHT, false}] = mono_input_monitor::KeyCode::Right;

    keyCodeMap_[{VK_INSERT, false}] = mono_input_monitor::KeyCode::Insert;
    keyCodeMap_[{VK_DELETE, false}] = mono_input_monitor::KeyCode::Del;
    keyCodeMap_[{VK_HOME, false}] = mono_input_monitor::KeyCode::Home;
    keyCodeMap_[{VK_END, false}] = mono_input_monitor::KeyCode::End;
    keyCodeMap_[{VK_PRIOR, false}] = mono_input_monitor::KeyCode::PageUp;
    keyCodeMap_[{VK_NEXT, false}] = mono_input_monitor::KeyCode::PageDown;
    keyCodeMap_[{VK_CAPITAL, false}] = mono_input_monitor::KeyCode::CapsLock;

    keyCodeMap_[{VK_F1, false}] = mono_input_monitor::KeyCode::F1;
    keyCodeMap_[{VK_F2, false}] = mono_input_monitor::KeyCode::F2;
    keyCodeMap_[{VK_F3, false}] = mono_input_monitor::KeyCode::F3;
    keyCodeMap_[{VK_F4, false}] = mono_input_monitor::KeyCode::F4;
    keyCodeMap_[{VK_F5, false}] = mono_input_monitor::KeyCode::F5;
    keyCodeMap_[{VK_F6, false}] = mono_input_monitor::KeyCode::F6;
    keyCodeMap_[{VK_F7, false}] = mono_input_monitor::KeyCode::F7;
    keyCodeMap_[{VK_F8, false}] = mono_input_monitor::KeyCode::F8;
    keyCodeMap_[{VK_F9, false}] = mono_input_monitor::KeyCode::F9;
    keyCodeMap_[{VK_F10, false}] = mono_input_monitor::KeyCode::F10;
    keyCodeMap_[{VK_F11, false}] = mono_input_monitor::KeyCode::F11;
    keyCodeMap_[{VK_F12, false}] = mono_input_monitor::KeyCode::F12;
    keyCodeMap_[{VK_F13, false}] = mono_input_monitor::KeyCode::F13;

    keyCodeMap_[{'0', false}] = mono_input_monitor::KeyCode::Alpha0;
    keyCodeMap_[{'1', false}] = mono_input_monitor::KeyCode::Alpha1;
    keyCodeMap_[{'2', false}] = mono_input_monitor::KeyCode::Alpha2;
    keyCodeMap_[{'3', false}] = mono_input_monitor::KeyCode::Alpha3;
    keyCodeMap_[{'4', false}] = mono_input_monitor::KeyCode::Alpha4;
    keyCodeMap_[{'5', false}] = mono_input_monitor::KeyCode::Alpha5;
    keyCodeMap_[{'6', false}] = mono_input_monitor::KeyCode::Alpha6;
    keyCodeMap_[{'7', false}] = mono_input_monitor::KeyCode::Alpha7;
    keyCodeMap_[{'8', false}] = mono_input_monitor::KeyCode::Alpha8;
    keyCodeMap_[{'9', false}] = mono_input_monitor::KeyCode::Alpha9;

    keyCodeMap_[{VK_NUMPAD0, false}] = mono_input_monitor::KeyCode::Numpad0;
    keyCodeMap_[{VK_NUMPAD1, false}] = mono_input_monitor::KeyCode::Numpad1;
    keyCodeMap_[{VK_NUMPAD2, false}] = mono_input_monitor::KeyCode::Numpad2;
    keyCodeMap_[{VK_NUMPAD3, false}] = mono_input_monitor::KeyCode::Numpad3;
    keyCodeMap_[{VK_NUMPAD4, false}] = mono_input_monitor::KeyCode::Numpad4;
    keyCodeMap_[{VK_NUMPAD5, false}] = mono_input_monitor::KeyCode::Numpad5;
    keyCodeMap_[{VK_NUMPAD6, false}] = mono_input_monitor::KeyCode::Numpad6;
    keyCodeMap_[{VK_NUMPAD7, false}] = mono_input_monitor::KeyCode::Numpad7;
    keyCodeMap_[{VK_NUMPAD8, false}] = mono_input_monitor::KeyCode::Numpad8;
    keyCodeMap_[{VK_NUMPAD9, false}] = mono_input_monitor::KeyCode::Numpad9;

    keyCodeMap_[{VK_OEM_1, false}] = mono_input_monitor::KeyCode::Oem1;
    keyCodeMap_[{VK_OEM_PLUS, false}] = mono_input_monitor::KeyCode::OemPlus;
    keyCodeMap_[{VK_OEM_COMMA, false}] = mono_input_monitor::KeyCode::OemComma;
    keyCodeMap_[{VK_OEM_MINUS, false}] = mono_input_monitor::KeyCode::OemMinus;
    keyCodeMap_[{VK_OEM_PERIOD, false}] = mono_input_monitor::KeyCode::OemPeriod;
    keyCodeMap_[{VK_OEM_2, false}] = mono_input_monitor::KeyCode::Oem2;
    keyCodeMap_[{VK_OEM_3, false}] = mono_input_monitor::KeyCode::Oem3;
    keyCodeMap_[{VK_OEM_4, false}] = mono_input_monitor::KeyCode::Oem4;
    keyCodeMap_[{VK_OEM_5, false}] = mono_input_monitor::KeyCode::Oem5;
    keyCodeMap_[{VK_OEM_6, false}] = mono_input_monitor::KeyCode::Oem6;
    keyCodeMap_[{VK_OEM_7, false}] = mono_input_monitor::KeyCode::Oem7;
    keyCodeMap_[{VK_OEM_102, false}] = mono_input_monitor::KeyCode::Oem102;
}

mono_input_monitor::KeyCode mono_input_monitor::KeyCodeConverter::Convert(WPARAM wParam, LPARAM lParam) const
{
    bool isExtended = (lParam & (1 << 24)) != 0;
    auto it = keyCodeMap_.find({wParam, isExtended});
    if (it != keyCodeMap_.end())
        return it->second;

    return mono_input_monitor::KeyCode::Null;
}

MONO_INPUT_MONITOR_API void mono_input_monitor::EditInputState
(
    KeyboardInputState &state, InputType inputType, KeyCode keyCode
){
    if (inputType == InputType::None || keyCode == KeyCode::Null)
        return; // Ignore None input type or Null key code

    // Casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (inputType == mono_input_monitor::InputType::Down && state.inputStates_[index] != mono_input_monitor::InputType::Press)
        state.inputStates_[index] = mono_input_monitor::InputType::Down;
    else if (inputType == mono_input_monitor::InputType::Up)
        state.inputStates_[index] = mono_input_monitor::InputType::Up;
}

MONO_INPUT_MONITOR_API void mono_input_monitor::UpdateInputState(KeyboardInputState &state)
{
    for (size_t i = 0; i < static_cast<size_t>(KeyCode::Size); ++i)
    {
        // After the key is pressed
        if (state.inputStates_[i] == mono_input_monitor::InputType::Down)
            state.inputStates_[i] = mono_input_monitor::InputType::Press; // Change to pressed state

        // After the key is up
        if (state.inputStates_[i] == mono_input_monitor::InputType::Up)
        {
            state.inputStates_[i] = mono_input_monitor::InputType::None; // Reset to none state

            // Set the last
            state.lastKeyCode_ = static_cast<KeyCode>(i);
            state.lastKeyPressTime_ = std::chrono::high_resolution_clock::now();
        }
    }
}

MONO_INPUT_MONITOR_API void mono_input_monitor::ResetInputState(KeyboardInputState &state)
{
    for (size_t i = 0; i < static_cast<size_t>(KeyCode::Size); ++i)
        state.inputStates_[i] = mono_input_monitor::InputType::None; // Reset all states to None

    state.lastKeyCode_ = KeyCode::Null; // Reset last key code
    state.lastKeyPressTime_ = std::chrono::high_resolution_clock::now(); // Reset last key press time
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetKey(const KeyboardInputState &state, KeyCode keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
        riaecs::NotifyError({"Invalid key code"}, RIAECS_LOG_LOC);

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (
        state.inputStates_[index] == mono_input_monitor::InputType::Press || 
        state.inputStates_[index] == mono_input_monitor::InputType::Down
    ) return true; // If the key is pressed or down, return true

    // If the key is not pressed or down, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetKeyDown(const KeyboardInputState &state, KeyCode keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
        riaecs::NotifyError({"Invalid key code"}, RIAECS_LOG_LOC);

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (state.inputStates_[index] == mono_input_monitor::InputType::Down)
        return true; // If the key is down, return true

    // If the key is not down, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetKeyUp(const KeyboardInputState &state, KeyCode keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
        riaecs::NotifyError({"Invalid key code"}, RIAECS_LOG_LOC);

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (state.inputStates_[index] == mono_input_monitor::InputType::Up)
        return true; // If the key is up, return true

    // If the key is not up, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetKeyDoubleTap(const KeyboardInputState &state, KeyCode keyCode, double threshold)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
        riaecs::NotifyError({"Invalid key code"}, RIAECS_LOG_LOC);

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (state.inputStates_[index] != mono_input_monitor::InputType::Up)
        return false; // Double-tap checked only when released

    if (
        state.lastKeyCode_ == keyCode &&
        std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - state.lastKeyPressTime_
        ).count() <= threshold
    ) return true; // If the last key is the same and the time difference is within the threshold, this is a double-tap

    return false;
}