#pragma once
#include "mono_input_monitor/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_input_monitor/include/input.h"

#include <unordered_map>
#include <Windows.h>
#include <chrono>

namespace mono_input_monitor
{
    enum class KeyCode : size_t
    {
        Null,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Return, Escape, Space, Tab, BackSpace,
        RAlt, LAlt, LShift, RShift, LCtrl, RCtrl,
        Up, Down, Left, Right,
        Insert, Del, Home, End, PageUp, PageDown, CapsLock,
        F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13,
        Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4, Numpad5, Numpad6, Numpad7, Numpad8, Numpad9, NumpadEnter,
        Oem1, OemPlus, OemComma, OemMinus, OemPeriod, Oem2, Oem3, Oem4, Oem5, Oem6, Oem7, Oem102,
        Size
    };

    class MONO_INPUT_MONITOR_API KeyInputConverter
    {
    private:
        std::unordered_map<UINT, InputType> inputTypeMap_;

    public:
        KeyInputConverter();
        ~KeyInputConverter() = default;

        InputType Convert(UINT msg) const;
    };

    class MONO_INPUT_MONITOR_API KeyCodeConverter
    {
    private:
        std::unordered_map<std::pair<WPARAM, bool>, KeyCode, riaecs::PairHash, riaecs::PairEqual> keyCodeMap_;

    public:
        KeyCodeConverter();
        ~KeyCodeConverter() = default;

        KeyCode Convert(WPARAM wParam, LPARAM lParam) const;
    };

    struct KeyboardInputState
    {
        InputType inputStates_[static_cast<size_t>(KeyCode::Size)];
        KeyCode lastKeyCode_ = KeyCode::Null;
        std::chrono::high_resolution_clock::time_point lastKeyPressTime_;
    };

    MONO_INPUT_MONITOR_API void EditInputState(KeyboardInputState& state, InputType inputType, KeyCode keyCode);
    MONO_INPUT_MONITOR_API void UpdateInputState(KeyboardInputState& state);
    MONO_INPUT_MONITOR_API void ResetInputState(KeyboardInputState& state);

    MONO_INPUT_MONITOR_API bool GetKey(const KeyboardInputState& state, KeyCode keyCode);
    MONO_INPUT_MONITOR_API bool GetKeyDown(const KeyboardInputState& state, KeyCode keyCode);
    MONO_INPUT_MONITOR_API bool GetKeyUp(const KeyboardInputState& state, KeyCode keyCode);
    MONO_INPUT_MONITOR_API bool GetKeyDoubleTap(const KeyboardInputState& state, KeyCode keyCode, double threshold = 0.3);

} // namespace mono_input_monitor