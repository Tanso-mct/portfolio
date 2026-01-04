#pragma once
#include "mono_input_monitor/include/dll_config.h"
#include "riaecs/riaecs.h"

#include "mono_input_monitor/include/input.h"

#include <Windows.h>
#include <chrono>

namespace mono_input_monitor
{
    constexpr UINT NOT_XBUTTON = 0;

    enum class MouseCode : size_t
    {
        Null, Left, Right, Middle, X1, X2, Wheel, Move, Size
    };

    class MONO_INPUT_MONITOR_API MouseInputConverter
    {
    private:
        std::unordered_map<UINT, InputType> inputTypeMap_;

    public:
        MouseInputConverter();
        ~MouseInputConverter() = default;

        InputType Convert(UINT msg) const;
    };

    class MONO_INPUT_MONITOR_API MouseCodeConverter
    {
    private:
        std::unordered_map<std::pair<UINT, UINT>, MouseCode, riaecs::PairHash, riaecs::PairEqual> mouseCodeMap_;

    public:
        MouseCodeConverter();
        ~MouseCodeConverter() = default;

        MouseCode Convert(UINT msg, WPARAM wParam) const;
    };

    struct MouseInputState
    {
        InputType inputStates_[static_cast<size_t>(MouseCode::Size)];
        MouseCode lastMouseCode_ = MouseCode::Null;
        std::chrono::high_resolution_clock::time_point lastMousePressTime_;

        POINT position_;
        POINT deltaPosition_;
        int wheelDelta_;
    };

    MONO_INPUT_MONITOR_API void EditInputState
    (
        MouseInputState& state, InputType inputType, MouseCode mouseCode, 
        WPARAM wParam, LPARAM lParam
    );
    MONO_INPUT_MONITOR_API void UpdateInputState(MouseInputState& state);
    MONO_INPUT_MONITOR_API void ResetInputState(MouseInputState& state);

    MONO_INPUT_MONITOR_API bool GetButton(const MouseInputState &state, MouseCode mouseCode);
    MONO_INPUT_MONITOR_API bool GetButtonDown(const MouseInputState &state, MouseCode mouseCode);
    MONO_INPUT_MONITOR_API bool GetButtonUp(const MouseInputState &state, MouseCode mouseCode);
    MONO_INPUT_MONITOR_API bool GetButtonDoubleTap(const MouseInputState &state, MouseCode mouseCode, double threshold = 0.3);

} // namespace mono_input_monitor