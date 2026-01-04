#pragma once

#include "windows_base/include/interfaces/monitor.h"

namespace wb
{
    /*******************************************************************************************************************
     * Table class to obtain codes identifying Keyboard keys and KeyCode from WPARAM and LPARAM.
     * Use this to manage key status, get key status, etc.
    /******************************************************************************************************************/

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

    class IKeyCodeTable
    {
    public:
        virtual ~IKeyCodeTable() = default;

        virtual void Create() = 0;
        virtual bool CheckIsReady() const = 0;

        virtual KeyCode GetKeyCode(WPARAM wParam, LPARAM lParam) const = 0;
    };

    /*******************************************************************************************************************
     * Context for managing Keyboard state
    /******************************************************************************************************************/

    class IKeyboardContext : public IContext
    {
    public:
        virtual ~IKeyboardContext() = default;

        virtual std::vector<UINT> &KeyStates() = 0;
        virtual KeyCode &PreviousKey() = 0;
        virtual std::chrono::high_resolution_clock::time_point &PreviousKeyTime() = 0;
    };

    /*******************************************************************************************************************
     * Keyboard monitor interface.
    /******************************************************************************************************************/

    class IKeyboardMonitor : public IMonitor
    {
    public:
        virtual ~IKeyboardMonitor() = default;

        virtual void SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable) = 0;
        virtual void SetKeyCodeTable(std::unique_ptr<IKeyCodeTable> keyCodeTable) = 0;

        virtual bool GetKey(const KeyCode &keyCode) = 0;
        virtual bool GetKeyDown(const KeyCode &keyCode) = 0;
        virtual bool GetKeyUp(const KeyCode &keyCode) = 0;
        virtual bool GetKeyDoubleTap(const KeyCode &keyCode, const double &threshold) = 0;
    };
} // namespace wb