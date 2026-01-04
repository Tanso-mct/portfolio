#pragma once

#include "windows_base/include/interfaces/monitor.h"

namespace wb
{
    constexpr UINT NOT_XBUTTON = 0;

    /*******************************************************************************************************************
     * Table class to obtain codes to identify Mouse buttons and MouseCode from WPARAM and LPARAM.
     * Use this to manage Mouse button status, get button status, etc.
    /******************************************************************************************************************/

    enum class MouseCode : size_t
    {
        Null, Left, Right, Middle, X1, X2, Size
    };

    class IMouseCodeTable
    {
    public:
        virtual ~IMouseCodeTable() = default;

        virtual void Create() = 0;
        virtual bool CheckIsReady() const = 0;

        virtual MouseCode GetMouseCode(UINT msg, WPARAM wParam) const = 0;
    };

    /*******************************************************************************************************************
     * Context for managing Mouse state
    /******************************************************************************************************************/

    class IMouseContext : public IContext
    {
    public:
        virtual ~IMouseContext() = default;

        virtual std::vector<UINT> &ButtonStates() = 0;
        virtual MouseCode &PreviousButton() = 0;
        virtual std::chrono::high_resolution_clock::time_point &PreviousButtonTime() = 0;

        virtual int &ScrollDelta() = 0;
        virtual int &PositionX() = 0;
        virtual int &PositionY() = 0;
        virtual int &DeltaPositionX() = 0;
        virtual int &DeltaPositionY() = 0;
    };

    /*******************************************************************************************************************
     * Mouse monitor interface.
    /******************************************************************************************************************/

    class IMouseMonitor : public IMonitor
    {
    public:
        virtual ~IMouseMonitor() = default;

        virtual void SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable) = 0;
        virtual void SetMouseCodeTable(std::unique_ptr<IMouseCodeTable> mouseCodeTable) = 0;

        virtual bool GetButton(const MouseCode &mouseCode) = 0;
        virtual bool GetButtonDown(const MouseCode &mouseCode) = 0;
        virtual bool GetButtonUp(const MouseCode &mouseCode) = 0;
        virtual bool GetButtonDoubleTap(const MouseCode &mouseCode, const double &threshold) = 0;

        virtual const int &GetScrollDelta() = 0;
        virtual const int &GetPositionX() = 0;
        virtual const int &GetPositionY() = 0;
        virtual const int &GetDeltaPositionX() = 0;
        virtual const int &GetDeltaPositionY() = 0;
    };

} // namespace wb