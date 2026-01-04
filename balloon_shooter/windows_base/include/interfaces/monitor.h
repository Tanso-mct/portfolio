#pragma once

#include "windows_base/include/interfaces/context.h"
#include "windows_base/include/interfaces/facade.h"

#include <vector>
#include <chrono>
#include <ctime>
#include <Windows.h>
#include <memory>
#include <unordered_map>

namespace wb
{
    /*******************************************************************************************************************
     * Constants for use in input state bitwise operations
    /******************************************************************************************************************/

    constexpr UINT INPUT_NONE = 0;
    constexpr UINT INPUT_DOWN = 1;
    constexpr UINT INPUT_PRESSED = 2;
    constexpr UINT INPUT_UP = 3;

    /*******************************************************************************************************************
     * Table to get it's input type from window messages
    /******************************************************************************************************************/

    class IInputTypeTable
    {
    public:
        virtual ~IInputTypeTable() = default;

        virtual void Create() = 0;
        virtual bool CheckIsReady() const = 0;

        virtual UINT GetInputType(UINT msg) const = 0;
    };

    /*******************************************************************************************************************
     * Monitor base interfaces.
     * Monitor must always inherit from IMonitor
    /******************************************************************************************************************/

    class IMonitor : public IFacade
    {
    public:
        virtual ~IMonitor() = default;

        virtual const size_t &GetFactoryID() const = 0;

        virtual void EditState(UINT msg, WPARAM wParam, LPARAM lParam) = 0;

        virtual void UpdateState() = 0;
        virtual void UpdateRawInputState(RAWINPUT* raw) = 0;
    };

} // namespace wb