#include "windows_base/src/pch.h"
#include "windows_base/include/monitor_mouse.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"
#include "windows_base/include/type_cast.h"

#include "windows_base/include/id_factory.h"
#include "windows_base/include/monitor_factory_registry.h"

void wb::MouseInputTypeTable::Create()
{
    if (isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseInputTypeTable is already created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Initialize the input type map with mouse messages
    inputTypeMap_[WM_LBUTTONDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_LBUTTONUP] = INPUT_UP;

    inputTypeMap_[WM_RBUTTONDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_RBUTTONUP] = INPUT_UP;

    inputTypeMap_[WM_MBUTTONDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_MBUTTONUP] = INPUT_UP;

    inputTypeMap_[WM_XBUTTONDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_XBUTTONUP] = INPUT_UP;

    isCreated_ = true;
}

bool wb::MouseInputTypeTable::CheckIsReady() const
{
    if (!isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseInputTypeTable is not created."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

UINT wb::MouseInputTypeTable::GetInputType(UINT msg) const
{
    bool isInputTypeFound = inputTypeMap_.find(msg) != inputTypeMap_.end();
    if (!isInputTypeFound)
    {
        // If the message is not found, return INPUT_NONE
        return INPUT_NONE;
    }

    // Return the input type for the given message
    return inputTypeMap_.at(msg);
}

void wb::MouseCodeTable::Create()
{
    if (isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseCodeTable is already created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Initialize the mouse code map with mouse messages and WPARAM values
    mouseCodeMap_[{WM_LBUTTONDOWN, wb::NOT_XBUTTON}] = wb::MouseCode::Left;
    mouseCodeMap_[{WM_LBUTTONUP, wb::NOT_XBUTTON}] = wb::MouseCode::Left;

    mouseCodeMap_[{WM_RBUTTONDOWN, wb::NOT_XBUTTON}] = wb::MouseCode::Right;
    mouseCodeMap_[{WM_RBUTTONUP, wb::NOT_XBUTTON}] = wb::MouseCode::Right;

    mouseCodeMap_[{WM_MBUTTONDOWN, wb::NOT_XBUTTON}] = wb::MouseCode::Middle;
    mouseCodeMap_[{WM_MBUTTONUP, wb::NOT_XBUTTON}] = wb::MouseCode::Middle;

    mouseCodeMap_[{WM_XBUTTONDOWN, XBUTTON1}] = wb::MouseCode::X1;
    mouseCodeMap_[{WM_XBUTTONUP, XBUTTON1}] = wb::MouseCode::X1;

    mouseCodeMap_[{WM_XBUTTONDOWN, XBUTTON2}] = wb::MouseCode::X2;
    mouseCodeMap_[{WM_XBUTTONUP, XBUTTON2}] = wb::MouseCode::X2;

    isCreated_ = true;
}

bool wb::MouseCodeTable::CheckIsReady() const
{
    if (!isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseCodeTable is not created."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

wb::MouseCode wb::MouseCodeTable::GetMouseCode(UINT msg, WPARAM wParam) const
{
    if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1 || GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
    {
        // If the message is XBUTTON, use the WPARAM to find the mouse code
        std::pair<UINT, UINT> key = {msg, GET_XBUTTON_WPARAM(wParam)};
        bool isMouseCodeFound = mouseCodeMap_.find(key) != mouseCodeMap_.end();
        if (!isMouseCodeFound)
        {
            // If the mouse code is not found, return MouseCode::Null
            return MouseCode::Null;
        }

        // Return the mouse code for the given message and WPARAM
        return mouseCodeMap_.at(key);
    }
    else
    {
        // For other messages, use the message only to find the mouse code
        std::pair<UINT, UINT> key = {msg, NOT_XBUTTON};
        bool isMouseCodeFound = mouseCodeMap_.find(key) != mouseCodeMap_.end();
        if (!isMouseCodeFound)
        {
            // If the mouse code is not found, return MouseCode::Null
            return MouseCode::Null;
        }

        // Return the mouse code for the given message
        return mouseCodeMap_.at(key);
    }
}

const WINDOWS_BASE_API size_t &wb::DefaultMouseMonitorFactoryID()
{
    static const size_t id = wb::IDFactory::CreateMonitorFactoryID();
    return id;
}

void wb::DefaultMouseMonitor::SetContext(std::unique_ptr<IContext> context)
{
    mouseContext_ = wb::UniqueAs<IMouseContext>(context);
    if (mouseContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to cast context to IMouseContext."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

bool wb::DefaultMouseMonitor::CheckIsReady() const
{
    if (inputTypeTable_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"InputTypeTable is not set."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (mouseCodeTable_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseCodeTable is not set."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (mouseContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseContext is not set."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

const size_t &wb::DefaultMouseMonitor::GetFactoryID() const
{
    return wb::DefaultMouseMonitorFactoryID();
}

void wb::DefaultMouseMonitor::EditState(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // If message is WM_MOUSEWHEEL
    if (msg == WM_MOUSEWHEEL)
    {
        // Update scroll delta
        mouseContext_->ScrollDelta() = GET_WHEEL_DELTA_WPARAM(wParam);
        return;
    }

    // If message is WM_MOUSEMOVE
    if (msg == WM_MOUSEMOVE)
    {
        mouseContext_->PositionX() = ((int)(short)LOWORD(lParam));
        mouseContext_->PositionY() = ((int)(short)HIWORD(lParam));
        return;
    }

    // Get the input type from the input type table
    UINT inputType = inputTypeTable_->GetInputType(msg);
    if (inputType == INPUT_NONE)
    {
        // If the input type is not found, return
        return;
    }

    // Get the mouse code from the mouse code table
    MouseCode mouseCode = mouseCodeTable_->GetMouseCode(msg, wParam);
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
    {
        // If the mouse code is not found, return
        return;
    }

    // Get the button states
    std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (inputType == INPUT_DOWN && buttonStates[index] != INPUT_PRESSED)
    {
        buttonStates[index] = INPUT_DOWN;
    }
    else if (inputType == INPUT_UP)
    {
        buttonStates[index] = INPUT_UP;
    }
}

void wb::DefaultMouseMonitor::UpdateState()
{
    // Get button states
    std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    for (size_t i = 0; i < static_cast<size_t>(MouseCode::Size); ++i)
    {
        // After the button is pressed
        if (buttonStates[i] == INPUT_DOWN)
        {
            buttonStates[i] = INPUT_PRESSED; // Change to pressed state
        }

        // After the button is up
        if (buttonStates[i] == INPUT_UP)
        {
            buttonStates[i] = INPUT_NONE; // Change to none state

            // Set previous button to current button
            mouseContext_->PreviousButton() = static_cast<MouseCode>(i);

            // Set previous button time to current time
            mouseContext_->PreviousButtonTime() = std::chrono::high_resolution_clock::now();
        }
    }

    // Reset scroll delta
    mouseContext_->ScrollDelta() = 0;

    // Reset delta position
    mouseContext_->DeltaPositionX() = 0;
    mouseContext_->DeltaPositionY() = 0;
}

void wb::DefaultMouseMonitor::UpdateRawInputState(RAWINPUT *raw)
{
    if (raw->header.dwType != RIM_TYPEMOUSE)
    {
        // If the raw input type is not mouse, do nothing
        return;
    }

    // Update delta position
    mouseContext_->DeltaPositionX() = raw->data.mouse.lLastX;
    mouseContext_->DeltaPositionY() = raw->data.mouse.lLastY;
}

void wb::DefaultMouseMonitor::SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable)
{
    if (inputTypeTable_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"InputTypeTable is already set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    inputTypeTable_ = std::move(inputTypeTable);
}

void wb::DefaultMouseMonitor::SetMouseCodeTable(std::unique_ptr<IMouseCodeTable> mouseCodeTable)
{
    if (mouseCodeTable_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"MouseCodeTable is already set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    mouseCodeTable_ = std::move(mouseCodeTable);
}

bool wb::DefaultMouseMonitor::GetButton(const MouseCode &mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid MouseCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get button states
    const std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (buttonStates[index] == INPUT_DOWN || buttonStates[index] == INPUT_PRESSED)
    {
        // If the button is down or pressed, return true
        return true;
    }

    // If the button is not down or pressed, return false
    return false;
}

bool wb::DefaultMouseMonitor::GetButtonDown(const MouseCode &mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid MouseCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get button states
    std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (buttonStates[index] == INPUT_DOWN)
    {
        // If the button is down, return true
        return true;
    }

    // If the button is not down, return false
    return false;
}

bool wb::DefaultMouseMonitor::GetButtonUp(const MouseCode &mouseCode)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid MouseCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get button states
    std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    // Casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (buttonStates[index] == INPUT_UP)
    {
        // If the button is up, return true
        return true;
    }

    // If the button is not up, return false
    return false;
}

bool wb::DefaultMouseMonitor::GetButtonDoubleTap(const MouseCode &mouseCode, const double &threshold)
{
    if (mouseCode == MouseCode::Null || mouseCode == MouseCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid MouseCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get button states
    std::vector<UINT> &buttonStates = mouseContext_->ButtonStates();

    // Get the casted index for the mouse code
    size_t index = static_cast<size_t>(mouseCode);

    if (buttonStates[index] != INPUT_UP)
    {
        // Double-tap checked only when released
        return false;
    }

    if 
    (
        mouseContext_->PreviousButton() == mouseCode &&
        std::chrono::duration<double>
        (
            std::chrono::high_resolution_clock::now() - mouseContext_->PreviousButtonTime()
        ).count() <= threshold
    ){
        // If the previous button is the same and the time difference is within the threshold, this is a double-tap
        return true;
    }

    return false;
}

std::unique_ptr<wb::IMonitor> wb::DefaultMouseMonitorFactory::Create() const
{
    std::unique_ptr<wb::IMonitor> monitor = std::make_unique<wb::DefaultMouseMonitor>();
    wb::DefaultMouseMonitor *mouseMonitor = wb::As<wb::DefaultMouseMonitor>(monitor.get());

    {
        std::unique_ptr<wb::IInputTypeTable> inputTypeTable = std::make_unique<wb::MouseInputTypeTable>();
        inputTypeTable->Create();
        if (!inputTypeTable->CheckIsReady())
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to create InputTypeTable."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        mouseMonitor->SetInputTypeTable(std::move(inputTypeTable));
    }

    {
        std::unique_ptr<wb::IMouseCodeTable> mouseCodeTable = std::make_unique<wb::MouseCodeTable>();
        mouseCodeTable->Create();
        if (!mouseCodeTable->CheckIsReady())
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to create MouseCodeTable."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        mouseMonitor->SetMouseCodeTable(std::move(mouseCodeTable));
    }

    {
        std::unique_ptr<wb::IMouseContext> mouseContext = std::make_unique<wb::MouseContext>();
        mouseContext->ButtonStates().resize(static_cast<size_t>(wb::MouseCode::Size), INPUT_NONE);

        mouseMonitor->SetContext(std::move(mouseContext));
    }

    if (!monitor->CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Monitor is not ready."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return monitor;
}

namespace wb
{
    WB_REGISTER_MONITOR_FACTORY(DefaultMouseMonitorFactory, DefaultMouseMonitorFactoryID())
}
