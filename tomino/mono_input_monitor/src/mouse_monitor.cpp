#include "mono_input_monitor/src/pch.h"
#include "mono_input_monitor/include/mouse_monitor.h"

mono_input_monitor::MouseInputConverter::MouseInputConverter()
{
    inputTypeMap_[WM_LBUTTONDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_LBUTTONUP] = mono_input_monitor::InputType::Up;

    inputTypeMap_[WM_RBUTTONDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_RBUTTONUP] = mono_input_monitor::InputType::Up;

    inputTypeMap_[WM_MBUTTONDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_MBUTTONUP] = mono_input_monitor::InputType::Up;

    inputTypeMap_[WM_XBUTTONDOWN] = mono_input_monitor::InputType::Down;
    inputTypeMap_[WM_XBUTTONUP] = mono_input_monitor::InputType::Up;
}

mono_input_monitor::InputType mono_input_monitor::MouseInputConverter::Convert(UINT msg) const
{
    auto it = inputTypeMap_.find(msg);
    if (it != inputTypeMap_.end())
        return it->second;

    return mono_input_monitor::InputType::None;
}

mono_input_monitor::MouseCodeConverter::MouseCodeConverter()
{
    mouseCodeMap_[{WM_LBUTTONDOWN, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Left;
    mouseCodeMap_[{WM_LBUTTONUP, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Left;

    mouseCodeMap_[{WM_RBUTTONDOWN, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Right;
    mouseCodeMap_[{WM_RBUTTONUP, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Right;

    mouseCodeMap_[{WM_MBUTTONDOWN, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Middle;
    mouseCodeMap_[{WM_MBUTTONUP, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Middle;

    mouseCodeMap_[{WM_XBUTTONDOWN, XBUTTON1}] = mono_input_monitor::MouseCode::X1;
    mouseCodeMap_[{WM_XBUTTONUP, XBUTTON1}] = mono_input_monitor::MouseCode::X1;

    mouseCodeMap_[{WM_XBUTTONDOWN, XBUTTON2}] = mono_input_monitor::MouseCode::X2;
    mouseCodeMap_[{WM_XBUTTONUP, XBUTTON2}] = mono_input_monitor::MouseCode::X2;

    mouseCodeMap_[{WM_MOUSEWHEEL, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Wheel;
    mouseCodeMap_[{WM_MOUSEMOVE, mono_input_monitor::NOT_XBUTTON}] = mono_input_monitor::MouseCode::Move;
}

mono_input_monitor::MouseCode mono_input_monitor::MouseCodeConverter::Convert(UINT msg, WPARAM wParam) const
{
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 || GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
    {
        // If the message is XBUTTON, use the WPARAM to find the mouse code
        std::pair<UINT, UINT> key = {msg, GET_XBUTTON_WPARAM(wParam)};
        bool isMouseCodeFound = mouseCodeMap_.find(key) != mouseCodeMap_.end();
        if (!isMouseCodeFound)
            return MouseCode::Null; // If the mouse code is not found, return MouseCode::Null

        // Return the mouse code for the given message and WPARAM
        return mouseCodeMap_.at(key);
    }
    else
    {
        // For other messages, use the message only to find the mouse code
        std::pair<UINT, UINT> key = {msg, NOT_XBUTTON};
        bool isMouseCodeFound = mouseCodeMap_.find(key) != mouseCodeMap_.end();
        if (!isMouseCodeFound)
            return MouseCode::Null; // If the mouse code is not found, return MouseCode::Null

        // Return the mouse code for the given message
        return mouseCodeMap_.at(key);
    }

    return mono_input_monitor::MouseCode::Null;
}

MONO_INPUT_MONITOR_API void mono_input_monitor::EditInputState
(
    MouseInputState &state, InputType inputType, MouseCode mouseCode, WPARAM wParam, LPARAM lParam
){
    // If message is WM_MOUSEWHEEL
    if (mouseCode == MouseCode::Wheel)
    {
        state.wheelDelta_ = GET_WHEEL_DELTA_WPARAM(wParam);
        return;
    }

    // If message is WM_MOUSEMOVE
    if (mouseCode == MouseCode::Move)
    {
        state.deltaPosition_.x = ((int)(short)LOWORD(lParam)) - state.position_.x;
        state.deltaPosition_.y = ((int)(short)HIWORD(lParam)) - state.position_.y;

        state.position_.x = ((int)(short)LOWORD(lParam));
        state.position_.y = ((int)(short)HIWORD(lParam));

        return;
    }

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (
        inputType == mono_input_monitor::InputType::Down && 
        state.inputStates_[index] != mono_input_monitor::InputType::Press
    ) state.inputStates_[index] = mono_input_monitor::InputType::Down;
    else if (inputType == mono_input_monitor::InputType::Up)
        state.inputStates_[index] = mono_input_monitor::InputType::Up;
}

MONO_INPUT_MONITOR_API void mono_input_monitor::UpdateInputState(MouseInputState &state)
{
    for (size_t i = 0; i < static_cast<size_t>(MouseCode::Size); ++i)
    {
        // After the button is pressed
        if (state.inputStates_[i] == mono_input_monitor::InputType::Down)
            state.inputStates_[i] = mono_input_monitor::InputType::Press; // Change to pressed state

        // After the button is up
        if (state.inputStates_[i] == mono_input_monitor::InputType::Up)
        {
            state.inputStates_[i] = mono_input_monitor::InputType::None; // Change to none state

            // Set the last
            state.lastMouseCode_ = static_cast<MouseCode>(i);
            state.lastMousePressTime_ = std::chrono::high_resolution_clock::now();
        }
    }

    state.wheelDelta_ = 0; // Reset wheel delta
    state.deltaPosition_.x = 0; // Reset delta position X
    state.deltaPosition_.y = 0; // Reset delta position Y
}

MONO_INPUT_MONITOR_API void mono_input_monitor::ResetInputState(MouseInputState &state)
{
    for (size_t i = 0; i < static_cast<size_t>(MouseCode::Size); ++i)
        state.inputStates_[i] = mono_input_monitor::InputType::None; // Reset all states to None

    state.lastMouseCode_ = MouseCode::Null; // Reset last mouse code
    state.lastMousePressTime_ = std::chrono::high_resolution_clock::now(); // Reset last mouse press time

    state.position_.x = 0; // Reset position X
    state.position_.y = 0; // Reset position Y
    state.deltaPosition_.x = 0; // Reset delta position X
    state.deltaPosition_.y = 0; // Reset delta position Y
    state.wheelDelta_ = 0; // Reset wheel delta
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetButton(const MouseInputState &state, MouseCode mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
        riaecs::NotifyError({"Invalid mouse code"}, RIAECS_LOG_LOC);

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (
        state.inputStates_[index] == mono_input_monitor::InputType::Down || 
        state.inputStates_[index] == mono_input_monitor::InputType::Press
    ) return true; // If the button is down or pressed, return true

    // If the button is not down or pressed, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetButtonDown(const MouseInputState &state, MouseCode mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
        riaecs::NotifyError({"Invalid mouse code"}, RIAECS_LOG_LOC);

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (state.inputStates_[index] == mono_input_monitor::InputType::Down)
        return true; // If the button is down, return true

    // If the button is not down, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetButtonUp(const MouseInputState &state, MouseCode mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
        riaecs::NotifyError({"Invalid mouse code"}, RIAECS_LOG_LOC);

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (state.inputStates_[index] == mono_input_monitor::InputType::Up)
        return true; // If the button is up, return true

    // If the button is not up, return false
    return false;
}

MONO_INPUT_MONITOR_API bool mono_input_monitor::GetButtonDoubleTap(
    const MouseInputState &state, MouseCode mouseCode, double threshold)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
        riaecs::NotifyError({"Invalid mouse code"}, RIAECS_LOG_LOC);

    // Get the casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (state.inputStates_[index] != mono_input_monitor::InputType::Up)
        return false; // Double-tap checked only when released

    if (
        state.lastMouseCode_ == mouseCode &&
        std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - state.lastMousePressTime_
        ).count() <= threshold
    ) return true; // If the last button is the same and the time difference is within the threshold, this is a double-tap

    return false;
}