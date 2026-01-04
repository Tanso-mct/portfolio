#include "windows_base/src/pch.h"
#include "windows_base/include/monitor_keyboard.h"

#include "windows_base/include/console_log.h"
#include "windows_base/include/error_handler.h"
#include "windows_base/include/type_cast.h"

#include "windows_base/include/id_factory.h"
#include "windows_base/include/monitor_factory_registry.h"

void wb::KeyboardInputTypeTable::Create()
{
    if (isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyboardInputTypeTable is already created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Initialize the input type map with keyboard messages
    inputTypeMap_[WM_KEYDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_KEYUP] = INPUT_UP;
    
    inputTypeMap_[WM_SYSKEYDOWN] = INPUT_DOWN;
    inputTypeMap_[WM_SYSKEYUP] = INPUT_UP;

    isCreated_ = true;
}

bool wb::KeyboardInputTypeTable::CheckIsReady() const
{
    if (!isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyboardInputTypeTable is not created."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

UINT wb::KeyboardInputTypeTable::GetInputType(UINT msg) const
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

void wb::KeyCodeTable::Create()
{
    if (isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyCodeTable is already created."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Initialize the key code map with key codes
    keyCodeMap_[{'A', false}] = wb::KeyCode::A;
    keyCodeMap_[{'B', false}] = wb::KeyCode::B;
    keyCodeMap_[{'C', false}] = wb::KeyCode::C;
    keyCodeMap_[{'D', false}] = wb::KeyCode::D;
    keyCodeMap_[{'E', false}] = wb::KeyCode::E;
    keyCodeMap_[{'F', false}] = wb::KeyCode::F;
    keyCodeMap_[{'G', false}] = wb::KeyCode::G;
    keyCodeMap_[{'H', false}] = wb::KeyCode::H;
    keyCodeMap_[{'I', false}] = wb::KeyCode::I;
    keyCodeMap_[{'J', false}] = wb::KeyCode::J;
    keyCodeMap_[{'K', false}] = wb::KeyCode::K;
    keyCodeMap_[{'L', false}] = wb::KeyCode::L;
    keyCodeMap_[{'M', false}] = wb::KeyCode::M;
    keyCodeMap_[{'N', false}] = wb::KeyCode::N;
    keyCodeMap_[{'O', false}] = wb::KeyCode::O;
    keyCodeMap_[{'P', false}] = wb::KeyCode::P;
    keyCodeMap_[{'Q', false}] = wb::KeyCode::Q;
    keyCodeMap_[{'R', false}] = wb::KeyCode::R;
    keyCodeMap_[{'S', false}] = wb::KeyCode::S;
    keyCodeMap_[{'T', false}] = wb::KeyCode::T;
    keyCodeMap_[{'U', false}] = wb::KeyCode::U;
    keyCodeMap_[{'V', false}] = wb::KeyCode::V;
    keyCodeMap_[{'W', false}] = wb::KeyCode::W;
    keyCodeMap_[{'X', false}] = wb::KeyCode::X;
    keyCodeMap_[{'Y', false}] = wb::KeyCode::Y;
    keyCodeMap_[{'Z', false}] = wb::KeyCode::Z;

    keyCodeMap_[{VK_RETURN, false}] = wb::KeyCode::Return;
    keyCodeMap_[{VK_ESCAPE, false}] = wb::KeyCode::Escape;
    keyCodeMap_[{VK_SPACE, false}] = wb::KeyCode::Space;
    keyCodeMap_[{VK_TAB, false}] = wb::KeyCode::Tab;
    keyCodeMap_[{VK_BACK, false}] = wb::KeyCode::BackSpace;

    keyCodeMap_[{VK_MENU, true}] = wb::KeyCode::RAlt;
    keyCodeMap_[{VK_MENU, false}] = wb::KeyCode::LAlt;
    keyCodeMap_[{VK_SHIFT, true}] = wb::KeyCode::RShift;
    keyCodeMap_[{VK_SHIFT, false}] = wb::KeyCode::LShift;
    keyCodeMap_[{VK_CONTROL, true}] = wb::KeyCode::RCtrl;
    keyCodeMap_[{VK_CONTROL, false}] = wb::KeyCode::LCtrl;

    keyCodeMap_[{VK_UP, false}] = wb::KeyCode::Up;
    keyCodeMap_[{VK_DOWN, false}] = wb::KeyCode::Down;
    keyCodeMap_[{VK_LEFT, false}] = wb::KeyCode::Left;
    keyCodeMap_[{VK_RIGHT, false}] = wb::KeyCode::Right;

    keyCodeMap_[{VK_INSERT, false}] = wb::KeyCode::Insert;
    keyCodeMap_[{VK_DELETE, false}] = wb::KeyCode::Del;
    keyCodeMap_[{VK_HOME, false}] = wb::KeyCode::Home;
    keyCodeMap_[{VK_END, false}] = wb::KeyCode::End;
    keyCodeMap_[{VK_PRIOR, false}] = wb::KeyCode::PageUp;
    keyCodeMap_[{VK_NEXT, false}] = wb::KeyCode::PageDown;
    keyCodeMap_[{VK_CAPITAL, false}] = wb::KeyCode::CapsLock;

    keyCodeMap_[{VK_F1, false}] = wb::KeyCode::F1;
    keyCodeMap_[{VK_F2, false}] = wb::KeyCode::F2;
    keyCodeMap_[{VK_F3, false}] = wb::KeyCode::F3;
    keyCodeMap_[{VK_F4, false}] = wb::KeyCode::F4;
    keyCodeMap_[{VK_F5, false}] = wb::KeyCode::F5;
    keyCodeMap_[{VK_F6, false}] = wb::KeyCode::F6;
    keyCodeMap_[{VK_F7, false}] = wb::KeyCode::F7;
    keyCodeMap_[{VK_F8, false}] = wb::KeyCode::F8;
    keyCodeMap_[{VK_F9, false}] = wb::KeyCode::F9;
    keyCodeMap_[{VK_F10, false}] = wb::KeyCode::F10;
    keyCodeMap_[{VK_F11, false}] = wb::KeyCode::F11;
    keyCodeMap_[{VK_F12, false}] = wb::KeyCode::F12;
    keyCodeMap_[{VK_F13, false}] = wb::KeyCode::F13;

    keyCodeMap_[{'0', false}] = wb::KeyCode::Alpha0;
    keyCodeMap_[{'1', false}] = wb::KeyCode::Alpha1;
    keyCodeMap_[{'2', false}] = wb::KeyCode::Alpha2;
    keyCodeMap_[{'3', false}] = wb::KeyCode::Alpha3;
    keyCodeMap_[{'4', false}] = wb::KeyCode::Alpha4;
    keyCodeMap_[{'5', false}] = wb::KeyCode::Alpha5;
    keyCodeMap_[{'6', false}] = wb::KeyCode::Alpha6;
    keyCodeMap_[{'7', false}] = wb::KeyCode::Alpha7;
    keyCodeMap_[{'8', false}] = wb::KeyCode::Alpha8;
    keyCodeMap_[{'9', false}] = wb::KeyCode::Alpha9;

    keyCodeMap_[{VK_NUMPAD0, false}] = wb::KeyCode::Numpad0;
    keyCodeMap_[{VK_NUMPAD1, false}] = wb::KeyCode::Numpad1;
    keyCodeMap_[{VK_NUMPAD2, false}] = wb::KeyCode::Numpad2;
    keyCodeMap_[{VK_NUMPAD3, false}] = wb::KeyCode::Numpad3;
    keyCodeMap_[{VK_NUMPAD4, false}] = wb::KeyCode::Numpad4;
    keyCodeMap_[{VK_NUMPAD5, false}] = wb::KeyCode::Numpad5;
    keyCodeMap_[{VK_NUMPAD6, false}] = wb::KeyCode::Numpad6;
    keyCodeMap_[{VK_NUMPAD7, false}] = wb::KeyCode::Numpad7;
    keyCodeMap_[{VK_NUMPAD8, false}] = wb::KeyCode::Numpad8;
    keyCodeMap_[{VK_NUMPAD9, false}] = wb::KeyCode::Numpad9;

    keyCodeMap_[{VK_OEM_1, false}] = wb::KeyCode::Oem1;
    keyCodeMap_[{VK_OEM_PLUS, false}] = wb::KeyCode::OemPlus;
    keyCodeMap_[{VK_OEM_COMMA, false}] = wb::KeyCode::OemComma;
    keyCodeMap_[{VK_OEM_MINUS, false}] = wb::KeyCode::OemMinus;
    keyCodeMap_[{VK_OEM_PERIOD, false}] = wb::KeyCode::OemPeriod;
    keyCodeMap_[{VK_OEM_2, false}] = wb::KeyCode::Oem2;
    keyCodeMap_[{VK_OEM_3, false}] = wb::KeyCode::Oem3;
    keyCodeMap_[{VK_OEM_4, false}] = wb::KeyCode::Oem4;
    keyCodeMap_[{VK_OEM_5, false}] = wb::KeyCode::Oem5;
    keyCodeMap_[{VK_OEM_6, false}] = wb::KeyCode::Oem6;
    keyCodeMap_[{VK_OEM_7, false}] = wb::KeyCode::Oem7;
    keyCodeMap_[{VK_OEM_102, false}] = wb::KeyCode::Oem102;

    isCreated_ = true;
}

bool wb::KeyCodeTable::CheckIsReady() const
{
    if (!isCreated_)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyCodeTable is not created."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

wb::KeyCode wb::KeyCodeTable::GetKeyCode(WPARAM wParam, LPARAM lParam) const
{
    // Check if the key is extended key
    bool isExtended = (lParam & (1 << 24)) != 0;

    // Check if the key code exists in the map
    std::pair<WPARAM, bool> keyPair = {wParam, isExtended};
    bool isKeyCodeFound = keyCodeMap_.find(keyPair) != keyCodeMap_.end();
    if (!isKeyCodeFound)
    {
        // If the key code is not found, return KeyCode::Null
        return wb::KeyCode::Null;
    }

    // Return the key code for the given WPARAM and LPARAM
    return keyCodeMap_.at(keyPair);
}

WINDOWS_BASE_API const size_t &wb::DefaultKeyboardMonitorFactoryID()
{
    static const size_t id = wb::IDFactory::CreateMonitorFactoryID();
    return id;
}

void wb::DefaultKeyboardMonitor::SetContext(std::unique_ptr<IContext> context)
{
    if (keyboardContext_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"DefaultKeyboardMonitor context is already set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Cast the context to IKeyboardContext
    keyboardContext_ = wb::UniqueAs<IKeyboardContext>(context);

    if (keyboardContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to cast context to IKeyboardContext."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }
}

bool wb::DefaultKeyboardMonitor::CheckIsReady() const
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

    if (keyCodeTable_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyCodeTable is not set."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    if (keyboardContext_ == nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyboardContext is not set."}
        );

        wb::ConsoleLogWrn(err);
        return false;
    }

    return true;
}

const size_t &wb::DefaultKeyboardMonitor::GetFactoryID() const
{
    return wb::DefaultKeyboardMonitorFactoryID();
}

void wb::DefaultKeyboardMonitor::EditState(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Get the input type from the input type table
    UINT inputType = inputTypeTable_->GetInputType(msg);
    if (inputType == INPUT_NONE)
    {
        // If the input type is not recognized, do nothing
        return;
    }

    // Get the key code from the key code table
    KeyCode keyCode = keyCodeTable_->GetKeyCode(wParam, lParam);
    if (keyCode == KeyCode::Null)
    {
        // If the key code is not recognized, do nothing
        return;
    }

    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    // Casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (inputType == INPUT_DOWN && keyStates[index] != INPUT_PRESSED)
    {
        keyStates[index] = INPUT_DOWN;
    }
    else if (inputType == INPUT_UP)
    {
        keyStates[index] = INPUT_UP;
    }
}

void wb::DefaultKeyboardMonitor::UpdateState()
{
    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    for (size_t i = 0; i < static_cast<size_t>(KeyCode::Size); ++i)
    {
        // After the key is pressed
        if (keyStates[i] == INPUT_DOWN)
        {
            keyStates[i] = INPUT_PRESSED; // Change to pressed state
        }

        // After the key is up
        if (keyStates[i] == INPUT_UP)
        {
            keyStates[i] = INPUT_NONE; // Change to none state

            // Set the previous key to the current key
            keyboardContext_->PreviousKey() = static_cast<KeyCode>(i);

            // Set the previous key time to the current time
            keyboardContext_->PreviousKeyTime() = std::chrono::high_resolution_clock::now();
        }
    }
}

void wb::DefaultKeyboardMonitor::UpdateRawInputState(RAWINPUT *raw)
{
}

void wb::DefaultKeyboardMonitor::SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable)
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

void wb::DefaultKeyboardMonitor::SetKeyCodeTable(std::unique_ptr<IKeyCodeTable> keyCodeTable)
{
    if (keyCodeTable_ != nullptr)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"KeyCodeTable is already set."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    keyCodeTable_ = std::move(keyCodeTable);
}

bool wb::DefaultKeyboardMonitor::GetKey(const KeyCode &keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid KeyCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (keyStates[index] == INPUT_PRESSED || keyStates[index] == INPUT_DOWN)
    {
        // If the key is pressed or down, return true
        return true;
    }

    // If the key is not pressed or down, return false
    return false;
}

bool wb::DefaultKeyboardMonitor::GetKeyDown(const KeyCode &keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid KeyCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (keyStates[index] == INPUT_DOWN)
    {
        // If the key is down, return true
        return true;
    }

    // If the key is not down, return false
    return false;
}

bool wb::DefaultKeyboardMonitor::GetKeyUp(const KeyCode &keyCode)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid KeyCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (keyStates[index] == INPUT_UP)
    {
        // If the key is up, return true
        return true;
    }

    // If the key is not up, return false
    return false;
}

bool wb::DefaultKeyboardMonitor::GetKeyDoubleTap(const KeyCode &keyCode, const double &threshold)
{
    if (keyCode == KeyCode::Null || keyCode == KeyCode::Size)
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Invalid KeyCode provided."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    // Get key states
    std::vector<UINT> &keyStates = keyboardContext_->KeyStates();

    // Get the casted index for the key code
    size_t index = static_cast<size_t>(keyCode);

    if (keyStates[index] != INPUT_UP)
    {
        // Double-tap checked only when released
        return false;
    }

    if 
    (
        keyboardContext_->PreviousKey() == keyCode &&
        std::chrono::duration<double>
        (
            std::chrono::high_resolution_clock::now() - keyboardContext_->PreviousKeyTime()
        ).count() <= threshold
    ){
        // If the previous key is the same and the time difference is within the threshold, this is a double-tap
        return true;
    }

    return false;
}

std::unique_ptr<wb::IMonitor> wb::DefaultKeyboardMonitorFactory::Create() const
{
    std::unique_ptr<wb::IMonitor> monitor = std::make_unique<wb::DefaultKeyboardMonitor>();
    wb::DefaultKeyboardMonitor *keyboardMonitor = wb::As<wb::DefaultKeyboardMonitor>(monitor.get());

    {
        std::unique_ptr<wb::IInputTypeTable> inputTypeTable = std::make_unique<wb::KeyboardInputTypeTable>();
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

        keyboardMonitor->SetInputTypeTable(std::move(inputTypeTable));
    }

    {
        std::unique_ptr<wb::IKeyCodeTable> keyCodeTable = std::make_unique<wb::KeyCodeTable>();
        keyCodeTable->Create();
        if (!keyCodeTable->CheckIsReady())
        {
            std::string err = wb::CreateErrorMessage
            (
                __FILE__, __LINE__, __FUNCTION__,
                {"Failed to create KeyCodeTable."}
            );

            wb::ConsoleLogErr(err);
            wb::ErrorNotify("WINDOWS_BASE", err);
            wb::ThrowRuntimeError(err);
        }

        keyboardMonitor->SetKeyCodeTable(std::move(keyCodeTable));
    }

    {
        std::unique_ptr<wb::IKeyboardContext> keyboardContext = std::make_unique<wb::KeyboardContext>();
        keyboardContext->KeyStates().resize(static_cast<size_t>(wb::KeyCode::Size), wb::INPUT_NONE);

        keyboardMonitor->SetContext(std::move(keyboardContext));
    }

    if (!monitor->CheckIsReady())
    {
        std::string err = wb::CreateErrorMessage
        (
            __FILE__, __LINE__, __FUNCTION__,
            {"Failed to create DefaultKeyboardMonitor."}
        );

        wb::ConsoleLogErr(err);
        wb::ErrorNotify("WINDOWS_BASE", err);
        wb::ThrowRuntimeError(err);
    }

    return monitor;
}

namespace wb
{
    WB_REGISTER_MONITOR_FACTORY(DefaultKeyboardMonitorFactory, DefaultKeyboardMonitorFactoryID())
}