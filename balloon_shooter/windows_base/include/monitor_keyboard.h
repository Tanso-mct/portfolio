#pragma once
#include "windows_base/include/dll_config.h"

#include "windows_base/include/interfaces/factory.h"
#include "windows_base/include/interfaces/monitor_keyboard.h"

namespace wb
{
    class WINDOWS_BASE_API KeyboardInputTypeTable : public IInputTypeTable
    {
    private:
        std::unordered_map<UINT, UINT> inputTypeMap_;
        bool isCreated_ = false;

    public:
        KeyboardInputTypeTable() = default;
        virtual ~KeyboardInputTypeTable() override = default;

        /***************************************************************************************************************
         * IInputTypeTable implementation
        /**************************************************************************************************************/

        void Create() override;
        bool CheckIsReady() const override;

        UINT GetInputType(UINT msg) const override;
    };

    struct KeyCodeTableHash 
    {
        template <typename T1, typename T2>
        std::size_t operator()(const std::pair<T1, T2>& pair) const 
        {
            return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
        }
    };

    struct KeyCodetableEqual 
    {
        template <typename T1, typename T2>
        bool operator()(const std::pair<T1, T2>& lhs, const std::pair<T1, T2>& rhs) const 
        {
            return lhs == rhs;
        }
    };

    class WINDOWS_BASE_API KeyCodeTable : public IKeyCodeTable
    {
    private:
        std::unordered_map<std::pair<WPARAM, bool>, KeyCode, KeyCodeTableHash, KeyCodetableEqual> keyCodeMap_;
        bool isCreated_ = false;

    public:
        KeyCodeTable() = default;
        virtual ~KeyCodeTable() override = default;

        /***************************************************************************************************************
         * IKeyCodeTable implementation
        /**************************************************************************************************************/

        void Create() override;
        bool CheckIsReady() const override;

        KeyCode GetKeyCode(WPARAM wParam, LPARAM lParam) const override;
    };

    class KeyboardContext : public IKeyboardContext
    {
    private:
        std::vector<UINT> keyStates_;
        KeyCode previousKey_ = KeyCode::Null;
        std::chrono::high_resolution_clock::time_point previousKeyTime_;

    public:
        KeyboardContext() = default;
        virtual ~KeyboardContext() override = default;

        /***************************************************************************************************************
         * IKeyboardContext implementation
        /**************************************************************************************************************/

        std::vector<UINT> &KeyStates() override { return keyStates_; }
        KeyCode &PreviousKey() override { return previousKey_; }
        std::chrono::high_resolution_clock::time_point &PreviousKeyTime() override { return previousKeyTime_; }
    };

    const WINDOWS_BASE_API size_t &DefaultKeyboardMonitorFactoryID();

    class WINDOWS_BASE_API DefaultKeyboardMonitor : public IKeyboardMonitor
    {
    private:
        std::unique_ptr<IInputTypeTable> inputTypeTable_ = nullptr;
        std::unique_ptr<IKeyCodeTable> keyCodeTable_ = nullptr;
        std::unique_ptr<IKeyboardContext> keyboardContext_ = nullptr;

    public:
        DefaultKeyboardMonitor() = default;
        virtual ~DefaultKeyboardMonitor() override = default;

        DefaultKeyboardMonitor(const DefaultKeyboardMonitor &) = delete;
        DefaultKeyboardMonitor &operator=(const DefaultKeyboardMonitor &) = delete;

        /***************************************************************************************************************
         * IFacade implementation
        /**************************************************************************************************************/

        void SetContext(std::unique_ptr<IContext> context) override;
        bool CheckIsReady() const override;

        /***************************************************************************************************************
         * IMonitor implementation
        /**************************************************************************************************************/

        const size_t &GetFactoryID() const override;

        void EditState(UINT msg, WPARAM wParam, LPARAM lParam) override;

        void UpdateState() override;
        void UpdateRawInputState(RAWINPUT* raw) override;

        /***************************************************************************************************************
         * IKeyboardMonitor implementation
        /**************************************************************************************************************/

        void SetInputTypeTable(std::unique_ptr<IInputTypeTable> inputTypeTable) override;
        void SetKeyCodeTable(std::unique_ptr<IKeyCodeTable> keyCodeTable) override;

        bool GetKey(const KeyCode &keyCode) override;
        bool GetKeyDown(const KeyCode &keyCode) override;
        bool GetKeyUp(const KeyCode &keyCode) override;
        bool GetKeyDoubleTap(const KeyCode &keyCode, const double &threshold) override;
    };

    class WINDOWS_BASE_API DefaultKeyboardMonitorFactory : public IMonitorFactory
    {
    public:
        DefaultKeyboardMonitorFactory() = default;
        virtual ~DefaultKeyboardMonitorFactory() override = default;

        std::unique_ptr<IMonitor> Create() const override;
    };

} // namespace wb